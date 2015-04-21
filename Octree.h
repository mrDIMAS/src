#pragma once


#include "Mesh.h"
#include "AABB.h"

//#define _OCTREE_DEBUG

class Octree {
public:
    class Triangle : public Mesh::Triangle {
    public:
        bool mRendered;
        explicit Triangle( unsigned short _a, unsigned short _b, unsigned short _c );
    };

    class Node {
    private:
        AABB mAABB;
        bool mSplit;
        vector< Triangle* > mTriangles;
        Node * mChild[ 8 ];
    public:
        friend class Octree;
        explicit Node();
        virtual ~Node();
        void AddTriangle( Triangle * t );
        void Split( );
    };

    Node * mRoot;
    Mesh * mMesh;
    int mSplitLimit;
    vector< Triangle * > mTriangleList;
    vector< Mesh::Triangle > mVisibleTriangleList;
    int mVisibleNodeCount;
    int mVisibleTriangleCount;
	ruVector3 GetAABBMin( vector< ruVector3 > & vertices );
	ruVector3 GetAABBMax( vector< ruVector3 > & vertices );
public:
    explicit Octree( Mesh * m, int _nodeSplitCriteria = 64 );
    virtual ~Octree();
    void PrepareTriangles( Node * node );
    void Build( vector< ruVector3 > & vertices, vector< Triangle* > & tris, Node * node );
    bool CubeInFrustum( const AABB & box );
    void GetVisibleTrianglesList( Node * node, vector< Mesh::Triangle > & triangles );
    vector< Mesh::Triangle > & GetTrianglesToRender(  );
    void NodeVisualize( Node * node );
    void VisualizeHierarchy();
};