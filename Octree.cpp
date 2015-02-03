#include "Octree.h"
#include "Camera.h"
#include "Vertex.h"

void Octree::VisualizeHierarchy() {
    NodeVisualize( mRoot );
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
    mVisibleTriangleList.clear();
    mVisibleNodeCount = 0;
    PrepareTriangles( mRoot );
    mVisibleTriangleCount = 0;

    GetVisibleTrianglesList( mRoot, mVisibleTriangleList );

    return mVisibleTriangleList;
}

void Octree::GetVisibleTrianglesList( Node * node, vector< Mesh::Triangle > & triangles ) {
    if( CubeInFrustum( node->mAABB )) {
        if( node->mSplit ) {
            for( int i = 0; i < 8; i++ ) { // go deeper, to child nodes and grab it visible triangles
                GetVisibleTrianglesList( node->mChild[ i ], triangles );
            }
        } else {
            mVisibleNodeCount++;

            for( auto t : node->mTriangles ) { // grab visible triangles
                if( !t->mRendered ) {
                    t->mRendered = true;

                    mVisibleTriangleCount++;

                    triangles.push_back( Mesh::Triangle( t->mA, t->mB, t->mC ));
                }
            }
        }
    }
}

bool Octree::CubeInFrustum( const AABB & box ) {
    return g_camera->frustum.IsAABBInside( box, ruVector3( mMesh->GetOwner()->mGlobalTransform.getOrigin().m_floats ) );
}

void Octree::Build( vector< ruVector3 > & vertices, vector< Triangle* > & tris, Node * node ) {
    if( tris.size() < mSplitLimit ) {
        for( int i = 0; i < tris.size(); i++ ) {
            node->AddTriangle( tris[ i ] );
        }

        return;
    }

    // Split node and process splitted
    node->Split();

    vector< Triangle* > leafTris[ 8 ];

    for( int childNum = 0; childNum < 8; childNum++ ) {
        Node * child = node->mChild[ childNum ];

        for( int i = 0; i < tris.size(); i++ ) {
            Triangle * tri = tris[ i ];

            ruVector3 a = vertices[ tri->mA ];
            ruVector3 b = vertices[ tri->mB ];
            ruVector3 c = vertices[ tri->mC ];

            if( child->mAABB.IsTriangleInside( a, b, c ) ) {
                leafTris[ childNum ].push_back( tri );
            }
        };
    }

    for( int childNum = 0; childNum < 8; childNum++ ) {
        if( leafTris[ childNum ].size() > 0 ) {
            Build( vertices, leafTris[ childNum ], node->mChild[ childNum ] );
        }
    }
}

Octree::~Octree() {
    delete mRoot;

    for( int i = 0; i < mTriangleList.size(); i++ ) {
        delete mTriangleList[ i ];
    }
}

Octree::Octree( Mesh * m, int _nodeSplitCriteria ) {
    mMesh = m;
    mSplitLimit = _nodeSplitCriteria;

    mRoot = new Node;

    vector< ruVector3 > vertices;
    for( auto & vertex : mMesh->mVertices ) {
        vertices.push_back( vertex.coords );
    }

    mRoot->mAABB = AABB( GetAABBMin( vertices ), GetAABBMax( vertices ) );

    for( auto & triangle : mMesh->mTriangles ) {
        mTriangleList.push_back( new Triangle( triangle.mA, triangle.mB, triangle.mC ) );
    }

    Build( vertices, mTriangleList, mRoot );
}

void Octree::PrepareTriangles( Node * node ) {
    if( node->mSplit ) {
        for( int i = 0; i < 8; i++ ) {
            PrepareTriangles( node->mChild[ i ] );
        }
    } else {
        for( int i = 0; i < node->mTriangles.size(); i++ ) {
            node->mTriangles[ i ]->mRendered = false;
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
    ruVector3 center = ( mAABB.mMin + mAABB.mMax ) * 0.5f;

    for(int i = 0; i < 8; i++) {
        mChild[i] = new Node();
    }

    mChild[0]->mAABB = AABB( mAABB.mMin, center );
    mChild[1]->mAABB = AABB( ruVector3( center.x, mAABB.mMin.y, mAABB.mMin.z ), ruVector3( mAABB.mMax.x, center.y, center.z ) );
    mChild[2]->mAABB = AABB( ruVector3( center.x, mAABB.mMin.y, center.z ), ruVector3( mAABB.mMax.x, center.y, mAABB.mMax.z ) );
    mChild[3]->mAABB = AABB( ruVector3( mAABB.mMin.x, mAABB.mMin.y, center.z ), ruVector3( center.x, center.y, mAABB.mMax.z ) );
    mChild[4]->mAABB = AABB( ruVector3( mAABB.mMin.x, center.y, mAABB.mMin.z), ruVector3( center.x, mAABB.mMax.y, center.z ) );
    mChild[5]->mAABB = AABB( ruVector3( center.x, center.y, mAABB.mMin.z), ruVector3( mAABB.mMax.x, mAABB.mMax.y, center.z ) );
    mChild[6]->mAABB = AABB( center, mAABB.mMax );
    mChild[7]->mAABB = AABB( ruVector3( mAABB.mMin.x, center.y, center.z ), ruVector3( center.x, mAABB.mMax.y, mAABB.mMax.z ) );

    mSplit = true;
}

void Octree::Node::AddTriangle( Triangle * t ) {
    mTriangles.push_back( t );
}

Octree::Node::~Node() {
    if( mSplit )
        for( int i = 0; i < 8; i++ ) {
            delete mChild[ i ];
        }
}

Octree::Node::Node() {
    mSplit = false;

    for(int i = 0; i < 8; i++) {
        mChild[i] = nullptr;
    }
}

Octree::Triangle::Triangle( unsigned short _a, unsigned short _b, unsigned short _c ) : Mesh::Triangle( _a, _b, _c ) {
    mRendered = false;
}
