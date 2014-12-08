#pragma once

#include "Game.h"

const float Infinite = FLT_MAX;

class GraphVertex;

struct Edge {
	GraphVertex * destVertex;
	float distToDestVertex;
	Edge();
	Edge( GraphVertex * destinationVertex, float distanceToDestinationVertex );
};



class GraphVertex {
private:
	bool used;	
	float distanceFromBegin;
	GraphVertex * ancestor;
	vector< Edge > edges;	

	float DistanceToVertex( GraphVertex * vertex );
public:
	friend class Pathfinder;

	ruVector3 position;

	explicit GraphVertex( ruVector3 pos );
	void ClearState( );
	void AddEdge( GraphVertex * vertex );
};

class Pathfinder {
private:
	vector< GraphVertex* > graph;
public:
	explicit Pathfinder();
	~Pathfinder();
	void SetVertices( vector< GraphVertex* > vertices );
	GraphVertex * GetPoint( int i );
	int GetPointCount( );
	void BuildPath( GraphVertex * begin, GraphVertex * end, vector< GraphVertex* > & outPoints );
	GraphVertex * GetVertexNearestTo( ruVector3 position, int * vertexNum = nullptr );
};

// helper class for Pathfinder
class Path {
public:
	vector< GraphVertex* > vertices;
	void AddPointAndLinkWithPrevious( GraphVertex * vertex );
	class NodeSorter {
	public:
		bool operator() ( const ruNodeHandle & node1, const ruNodeHandle & node2 );
	};
	void ScanSceneForPath( ruNodeHandle scene, string pathBaseName );
};