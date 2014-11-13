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

	Vector3 position;

	explicit GraphVertex( Vector3 pos );
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
	GraphVertex * GetVertexNearestTo( Vector3 position, int * vertexNum = nullptr );
};

// helper class for Pathfinder
class Path {
public:
	vector< GraphVertex* > vertices;
	void AddPointAndLinkWithPrevious( GraphVertex * vertex );
	class NodeSorter {
	public:
		bool operator() ( const NodeHandle & node1, const NodeHandle & node2 );
	};
	void ScanSceneForPath( NodeHandle scene, string pathBaseName );
};