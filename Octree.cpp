#include "Octree.h"
#include "Camera.h"
#include "Vertex.h"

void Octree::VisualizeHierarchy() {
    NodeVisualize( root );
}

void Octree::NodeVisualize( Node * node ) {
    if( node->divided ) {
        for( int i = 0; i < 8; i++ ) {
            NodeVisualize( node->childs[ i ]);
        }
    } else {
        node->box.Visualize();
    }
}

vector< Mesh::Triangle > & Octree::GetTrianglesToRender() {
    visibleTris.clear();
    visibleNodes = 0;
    PrepareTriangles( root );
    visibleTriangles = 0;

    GetVisibleTrianglesList( root, visibleTris );

    return visibleTris;
}

void Octree::GetVisibleTrianglesList( Node * node, vector< Mesh::Triangle > & triangles ) {
    if( CubeInFrustum( node->box )) {
        if( node->divided ) {
            for( int i = 0; i < 8; i++ ) { // go deeper, to child nodes and grab it visible triangles
                GetVisibleTrianglesList( node->childs[ i ], triangles );
            }
        } else {
            visibleNodes++;

            for( auto t : node->triangles ) { // grab visible triangles
                if( !t->rendered ) {
                    t->rendered = true;

                    visibleTriangles++;

                    triangles.push_back( Mesh::Triangle( t->a, t->b, t->c ));
                }
            }
        }
    }
}

bool Octree::CubeInFrustum( const AABB & box ) {
    btVector3 pos = mesh->GetParentNode()->globalTransform.getOrigin();

    for( int planeNum = 0; planeNum < 6; planeNum++ ) {
        bool nextPlane = false;

        for( int vertexNum = 0; vertexNum < 8; vertexNum++ ) {
            D3DXVECTOR4 vertex = D3DXVECTOR4( box.vertices[ vertexNum ].x + pos.x(), box.vertices[ vertexNum ].y + pos.y(), box.vertices[ vertexNum ].z + pos.z(), 1 );

            if( D3DXPlaneDot( &g_camera->frustumPlanes[ planeNum ], &vertex ) > 0 ) {
                nextPlane = true;

                break;
            }
        }

        if( nextPlane ) {
            continue;
        }

        return false;
    }

    return true;
}

void Octree::Build( vector< Vector3 > & vertices, vector< Triangle* > & tris, Node * node ) {
    if( tris.size() < nodeSplitCriteria ) {
        for( int i = 0; i < tris.size(); i++ ) {
            node->AddTriangle( tris[ i ] );
        }

        return;
    }

    // Split node and process splitted
    node->Split();

    vector< Triangle* > leafTris[ 8 ];

    for( int childNum = 0; childNum < 8; childNum++ ) {
        Node * child = node->childs[ childNum ];

        for( int i = 0; i < tris.size(); i++ ) {
            Triangle * tri = tris[ i ];

            Vector3 a = vertices[ tri->a ];
            Vector3 b = vertices[ tri->b ];
            Vector3 c = vertices[ tri->c ];

            if( child->box.IsTriangleInside( a, b, c ) ) {
                leafTris[ childNum ].push_back( tri );
            }
        };
    }

    for( int childNum = 0; childNum < 8; childNum++ ) {
        if( leafTris[ childNum ].size() > 0 ) {
            Build( vertices, leafTris[ childNum ], node->childs[ childNum ] );
        }
    }
}

Octree::~Octree() {
    delete root;

    for( int i = 0; i < triangles.size(); i++ ) {
        delete triangles[ i ];
    }
}

Octree::Octree( Mesh * m, int _nodeSplitCriteria ) {
    mesh = m;
    nodeSplitCriteria = _nodeSplitCriteria;

    root = new Node;

    vector< Vector3 > vertices;
    for( int i = 0; i < mesh->vertices.size(); i++ ) {
        vertices.push_back( mesh->vertices[ i ].coords );
    }



    root->box = AABB( GetAABBMin( vertices ), GetAABBMax( vertices ) );

    for( auto & triangle : mesh->triangles ) {
        triangles.push_back( new Triangle( triangle.a, triangle.b, triangle.c ) );
    }

    Build( vertices, triangles, root );
}

void Octree::PrepareTriangles( Node * node ) {
    if( node->divided ) {
        for( int i = 0; i < 8; i++ ) {
            PrepareTriangles( node->childs[ i ] );
        }
    } else {
        for( int i = 0; i < node->triangles.size(); i++ ) {
            node->triangles[ i ]->rendered = false;
        }
    }
}

Vector3 Octree::GetAABBMax( vector< Vector3 > & vertices ) {
    Vector3 max = Vector3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

    for( size_t i = 0; i < vertices.size(); i++ ) {
        Vector3 v = vertices[ i ];

        if( v.x > max.x ) {
            max.x = v.x;
        }

        if( v.y > max.y ) {
            max.y = v.y;
        }

        if( v.z > max.z ) {
            max.z = v.z;
        }
    }

    return max;
}

Vector3 Octree::GetAABBMin( vector< Vector3 > & vertices ) {
    Vector3 min = Vector3( FLT_MAX, FLT_MAX, FLT_MAX );

    for( size_t i = 0; i < vertices.size(); i++ ) {
        Vector3 v = vertices[ i ];

        if( v.x < min.x ) {
            min.x = v.x;
        }

        if( v.y < min.y ) {
            min.y = v.y;
        }

        if( v.z < min.z ) {
            min.z = v.z;
        }
    }

    return min;
}

void Octree::Node::Split() {
    Vector3 center = ( box.min + box.max ) * 0.5f;

    for(int i = 0; i < 8; i++) {
        childs[i] = new Node();
    }

    childs[0]->box = AABB( box.min, center );
    childs[1]->box = AABB( Vector3( center.x, box.min.y, box.min.z ), Vector3( box.max.x, center.y, center.z ) );
    childs[2]->box = AABB( Vector3( center.x, box.min.y, center.z ), Vector3( box.max.x, center.y, box.max.z ) );
    childs[3]->box = AABB( Vector3( box.min.x, box.min.y, center.z ), Vector3( center.x, center.y, box.max.z ) );
    childs[4]->box = AABB( Vector3( box.min.x, center.y, box.min.z), Vector3( center.x, box.max.y, center.z ) );
    childs[5]->box = AABB( Vector3( center.x, center.y, box.min.z), Vector3( box.max.x, box.max.y, center.z ) );
    childs[6]->box = AABB( center, box.max );
    childs[7]->box = AABB( Vector3( box.min.x, center.y, center.z ), Vector3( center.x, box.max.y, box.max.z ) );

    divided = true;
}

void Octree::Node::AddTriangle( Triangle * t ) {
    triangles.push_back( t );
}

Octree::Node::~Node() {
    if( divided )
        for( int i = 0; i < 8; i++ ) {
            delete childs[ i ];
        }
}

Octree::Node::Node() {
    divided = false;

    for(int i = 0; i < 8; i++) {
        childs[i] = nullptr;
    }
}

Octree::Triangle::Triangle( unsigned short _a, unsigned short _b, unsigned short _c ) : Mesh::Triangle( _a, _b, _c ) {
    rendered = false;
}
