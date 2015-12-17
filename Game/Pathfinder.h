#pragma once

#include "Game.h"

const float Infinite = FLT_MAX;

class GraphVertex;

class Edge {
public:
    GraphVertex * mpDestVertex;
    float mDistToDestVertex;
    explicit Edge();
    explicit Edge( GraphVertex * destinationVertex, float distanceToDestinationVertex );
};

class GraphVertex {
private:
    bool mUsed;
    float mDistanceFromBegin;
    GraphVertex * mAncestor;
    vector< Edge > mEdges;
    float DistanceToVertex( GraphVertex * vertex );
public:
    friend class Pathfinder;
    ruVector3 mPosition;
    explicit GraphVertex( ruVector3 pos );
    void ClearState( );
    void AddEdge( GraphVertex * vertex );
};

class Pathfinder {
private:
    vector< GraphVertex* > mGraph;
public:
    explicit Pathfinder();
    virtual ~Pathfinder();
    void SetVertices( vector< GraphVertex* > vertices );
    GraphVertex * GetPoint( int i );
    int GetPointCount( );
    void BuildPath( GraphVertex * begin, GraphVertex * end, vector< GraphVertex* > & outPoints );
    GraphVertex * GetVertexNearestTo( ruVector3 position, int * vertexNum = nullptr );
};

// helper class for Pathfinder
class Path {
public:
    vector< GraphVertex* > mVertexList;
    void AddPointAndLinkWithPrevious( GraphVertex * vertex );
    class NodeSorter {
    public:
        bool operator() ( const ruSceneNode * node1, const ruSceneNode * node2 );
    };
    void BuildPath( ruSceneNode * scene, string pathBaseName );
};