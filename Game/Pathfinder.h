#pragma once

#include "Game.h"

const float Infinite = FLT_MAX;

class GraphVertex;

class Edge {
public:
	weak_ptr<GraphVertex> mpDestVertex;
	float mDistToDestVertex;
	explicit Edge();
	explicit Edge(const shared_ptr<GraphVertex> & destinationVertex, float distanceToDestinationVertex);
};

class GraphVertex : public enable_shared_from_this<GraphVertex> {
private:
	bool mUsed;
	float mDistanceFromBegin;
	weak_ptr<GraphVertex> mAncestor;
	vector< Edge > mEdges;
	float DistanceToVertex(const shared_ptr<GraphVertex> & vertex);
public:
	friend class Pathfinder;
	shared_ptr<ruSceneNode> mNode;
	explicit GraphVertex(const shared_ptr<ruSceneNode> & node);
	void ClearState();
	void AddEdge(const shared_ptr<GraphVertex> & vertex);
};

class Pathfinder {
private:
	vector<shared_ptr<GraphVertex>> mGraph;
public:
	explicit Pathfinder();
	virtual ~Pathfinder();
	void SetVertices(const vector<shared_ptr<GraphVertex>> & vertices);
	shared_ptr<GraphVertex> GetPoint(int i);
	int GetPointCount();
	void BuildPath(const shared_ptr<GraphVertex> & begin, const shared_ptr<GraphVertex> & end, vector<shared_ptr<GraphVertex>> & outPoints);
	shared_ptr<GraphVertex> GetVertexNearestTo(ruVector3 position, int * vertexNum = nullptr);
};

// helper class for Pathfinder
class Path {
public:
	Path() {

	}
	Path(shared_ptr<ruSceneNode> scene, string pathBaseName);
	vector<shared_ptr<GraphVertex>> mVertexList;
	void AddPointAndLinkWithPrevious(const shared_ptr<GraphVertex> & vertex);
	class NodeSorter {
	public:
		bool operator() (const shared_ptr<ruSceneNode> node1, const shared_ptr<ruSceneNode> node2);
	};
	shared_ptr<GraphVertex> Get(const string & name) {
		for (auto & v : mVertexList) {
			if (v->mNode->GetName() == name) {
				return v;
			}
		}
		throw runtime_error(StringBuilder("Path point '") << name << "' not found!");
		return nullptr;
	}
	void operator += (const Path & path) {
		mVertexList.insert(mVertexList.end(), path.mVertexList.begin(), path.mVertexList.end());
	}
};