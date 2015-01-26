#include "Octree.h"
#include "Camera.h"
#include "Vertex.h"

void Octree::VisualizeHierarchy() {
    NodeVisualize( root );
}

void Octree::NodeVisualize( Node * node ) {
    /*
    if( node->divided ) {
        for( int i = 0; i < 8; i++ ) {
            NodeVisualize( node->childs[ i ]);
        }
    } else {
        node->box.Visualize();
    }*/
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
    return g_camera->frustum.IsAABBInside( box, ruVector3( mesh->GetParentNode()->globalTransform.getOrigin().m_floats ) );
}

void Octree::Build( vector< ruVector3 > & vertices, vector< Triangle* > & tris, Node * node ) {
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

            ruVector3 a = vertices[ tri->a ];
            ruVector3 b = vertices[ tri->b ];
            ruVector3 c = vertices[ tri->c ];

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

    vector< ruVector3 > vertices;
    for( auto & vertex : mesh->vertices ) {
        vertices.push_back( vertex.coords );
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

ruVector3 Octree::GetAABBMax( vector< ruVector3 > & vertices ) {
    ruVector3 max = ruVector3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

    for( size_t i = 0; i < vertices.size(); i++ ) {
        ruVector3 v = vertices[ i ];

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

ruVector3 Octree::GetAABBMin( vector< ruVector3 > & vertices ) {
    ruVector3 min = ruVector3( FLT_MAX, FLT_MAX, FLT_MAX );

    for( size_t i = 0; i < vertices.size(); i++ ) {
        ruVector3 v = vertices[ i ];

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
    ruVector3 center = ( box.mMin + box.mMax ) * 0.5f;

    for(int i = 0; i < 8; i++) {
        childs[i] = new Node();
    }

    childs[0]->box = AABB( box.mMin, center );
    childs[1]->box = AABB( ruVector3( center.x, box.mMin.y, box.mMin.z ), ruVector3( box.mMax.x, center.y, center.z ) );
    childs[2]->box = AABB( ruVector3( center.x, box.mMin.y, center.z ), ruVector3( box.mMax.x, center.y, box.mMax.z ) );
    childs[3]->box = AABB( ruVector3( box.mMin.x, box.mMin.y, center.z ), ruVector3( center.x, center.y, box.mMax.z ) );
    childs[4]->box = AABB( ruVector3( box.mMin.x, center.y, box.mMin.z), ruVector3( center.x, box.mMax.y, center.z ) );
    childs[5]->box = AABB( ruVector3( center.x, center.y, box.mMin.z), ruVector3( box.mMax.x, box.mMax.y, center.z ) );
    childs[6]->box = AABB( center, box.mMax );
    childs[7]->box = AABB( ruVector3( box.mMin.x, center.y, center.z ), ruVector3( center.x, box.mMax.y, box.mMax.z ) );

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
