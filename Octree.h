#pragma once

#include "Common.h"
#include "Mesh.h"
#include "AABB.h"

//#define _OCTREE_DEBUG

class Octree
{
public:
    IDirect3DIndexBuffer9 * indices;

    class Triangle : public Mesh::Triangle
    {
    public:
        bool rendered;

        Triangle( unsigned short _a, unsigned short _b, unsigned short _c );
    };

    class Node
    {
    private:
        AABB box;

        bool divided;

        vector< Triangle* > triangles;

        Node * childs[ 8 ];
    public:

        friend class Octree;

        Node();
        virtual ~Node();
        void AddTriangle( Triangle * t );
        void Split( );
    };

    Node * root;
    Mesh * mesh;
    int nodeSplitCriteria;

    vector< Triangle * > triangles;

    ruVector3 GetAABBMin( vector< ruVector3 > & vertices );
    ruVector3 GetAABBMax( vector< ruVector3 > & vertices );

    vector< Mesh::Triangle > visibleTris;
    int visibleNodes;
    int visibleTriangles;
public:
    Octree( Mesh * m, int _nodeSplitCriteria = 64 );
    ~Octree();
    void PrepareTriangles( Node * node );
    void Build( vector< ruVector3 > & vertices, vector< Triangle* > & tris, Node * node );
    bool CubeInFrustum( const AABB & box );
    void GetVisibleTrianglesList( Node * node, vector< Mesh::Triangle > & triangles );
    vector< Mesh::Triangle > & GetTrianglesToRender(  );
    void NodeVisualize( Node * node );
    void VisualizeHierarchy();
};