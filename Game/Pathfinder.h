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


/*
int main() {
	// route test
	vector< GraphVertex* > vertex;

	vertex.push_back( new GraphVertex( 0, 0, 0 ));
	vertex.push_back( new GraphVertex( 1, 2, 0 ));
	vertex.push_back( new GraphVertex( 1, 5, 0 ));
	vertex.push_back( new GraphVertex( 4, 4, 0 ));
	vertex.push_back( new GraphVertex( 4, 2, 0 ));
	vertex.push_back( new GraphVertex( 7, 2, 0 ));

	vertex[0]->AddEdge( vertex[1] );	

	vertex[1]->AddEdge( vertex[2] );	
	vertex[1]->AddEdge( vertex[4] );	

	vertex[2]->AddEdge( vertex[3] );	
	vertex[3]->AddEdge( vertex[4] );	

	vertex[4]->AddEdge( vertex[5] );	

	Pathfinder testRoute;

	for( auto v : vertex )
		testRoute.AddPoint( v );

	GraphVertex * begin = vertex[1];
	GraphVertex * end = vertex[3];

	vector< GraphVertex* > out;
	testRoute.BuildPath( begin, end, out );


}*/