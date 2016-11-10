#include "Precompiled.h"

#include "Pathfinder.h"

void Pathfinder::BuildPath(const shared_ptr<GraphVertex> & begin, const shared_ptr<GraphVertex> & end, vector<shared_ptr<GraphVertex>> & outPoints) {
	// this is modified Dijkstra path-finding algorithm
	assert(begin != nullptr);
	assert(end != nullptr);
	assert(mGraph.size() != 0);
	outPoints.clear();
	// clear state of all vertices
	for (auto vertex : mGraph) {
		vertex->ClearState();
	}
	// set begin graph vertex
	begin->mDistanceFromBegin = 0;

	for (int i = 0; i < mGraph.size(); i++) {
		// get nearest vertex
		shared_ptr<GraphVertex> nearest;
		for (auto vertex : mGraph) {
			if (vertex->mUsed) {
				continue;
			} else if (!nearest) {
				nearest = vertex;
			} else if (vertex->mDistanceFromBegin < nearest->mDistanceFromBegin) {
				nearest = vertex;
			}
		}

		if (nearest->mDistanceFromBegin >= Infinite) {
			break;
		}

		nearest->mUsed = true;

		// relaxation
		for (auto & edge : nearest->mEdges) {
			auto destVertex = edge.mpDestVertex.lock();
			if (destVertex) {
				if (nearest->mDistanceFromBegin + edge.mDistToDestVertex < destVertex->mDistanceFromBegin) {
					destVertex->mDistanceFromBegin = nearest->mDistanceFromBegin + edge.mDistToDestVertex;
					destVertex->mAncestor = nearest;
				}
			}
		}
	}

	// restore path to dest vertex
	for (shared_ptr<GraphVertex> v = end; v != begin; ) {
		outPoints.push_back(v);

		if (!v->mAncestor.expired()) {
			v = v->mAncestor.lock();
		} else {
			throw runtime_error(StringBuilder("Unable to build path. Vertex ") << v->mNode->GetName() << " isolated!");
		}
	}
	outPoints.push_back(begin);
	reverse(outPoints.begin(), outPoints.end());
}

int Pathfinder::GetPointCount() {
	return mGraph.size();
}

shared_ptr<GraphVertex> Pathfinder::GetPoint(int i) {
	if (i < 0 || i >= mGraph.size()) {
		return nullptr;
	} else {
		return mGraph[i];
	}
}

void Pathfinder::SetVertices(const vector<shared_ptr<GraphVertex>> & vertices) {
	mGraph = vertices;
}

Pathfinder::~Pathfinder() {
}

Pathfinder::Pathfinder() {
}

shared_ptr<GraphVertex> Pathfinder::GetVertexNearestTo(ruVector3 position, int * vertexNum) {
	assert(mGraph.size() != 0);
	shared_ptr<GraphVertex> nearest = mGraph.front();
	int nearestIndex = 0, index = 0;;
	for (auto & vertex : mGraph) {
		if ((vertex->mNode->GetPosition() - position).Length2() < (nearest->mNode->GetPosition() - position).Length2()) {
			nearest = vertex;
			nearestIndex = index;
		}
		index++;
	}
	if (vertexNum) {
		*vertexNum = nearestIndex;
	}
	return nearest;
}

void GraphVertex::AddEdge(const shared_ptr<GraphVertex> & vertex) {
	mEdges.push_back(Edge(vertex, DistanceToVertex(vertex)));
	vertex->mEdges.push_back(Edge(shared_from_this(), DistanceToVertex(vertex)));
}

void GraphVertex::ClearState() {
	mAncestor.reset();
	mUsed = false;
	mDistanceFromBegin = Infinite;
}

GraphVertex::GraphVertex(const shared_ptr<ruSceneNode> & node) : mNode(node) {	
	ClearState();
}

float GraphVertex::DistanceToVertex(const shared_ptr<GraphVertex> & vertex) {
	return (mNode->GetPosition() - vertex->mNode->GetPosition()).Length();
}

Edge::Edge(const shared_ptr<GraphVertex> & destinationVertex, float distanceToDestinationVertex) {
	mpDestVertex = destinationVertex;
	mDistToDestVertex = distanceToDestinationVertex;
}

Edge::Edge() {
	mpDestVertex.reset();
	mDistToDestVertex = Infinite;
}

Path::Path(shared_ptr<ruSceneNode> scene, string pathBaseName) {
	vector<shared_ptr<ruSceneNode>> pointNodes;
	for (int i = 0; i < scene->GetCountChildren(); i++) {
		shared_ptr<ruSceneNode> child = scene->GetChild(i);
		string cName = child->GetName();
		if (cName.size() < pathBaseName.size()) {
			continue;
		}
		int numPos = cName.find_first_of("0123456789");
		if (numPos > pathBaseName.size()) {
			continue;
		}
		string bName = cName.substr(0, pathBaseName.size());
		if (bName == pathBaseName) {
			pointNodes.push_back(child);
		}
	}
	NodeSorter nodeSorter;
	sort(pointNodes.begin(), pointNodes.end(), nodeSorter);
	for (auto node : pointNodes) {
		AddPointAndLinkWithPrevious(make_shared<GraphVertex>(node));
	}
}

void Path::AddPointAndLinkWithPrevious(const shared_ptr<GraphVertex> & vertex) {
	if (mVertexList.size() > 0) {
		auto prev = mVertexList[mVertexList.size() - 1];
		prev->AddEdge(vertex);
	}
	mVertexList.push_back(vertex);
}

bool Path::NodeSorter::operator()(const shared_ptr<ruSceneNode> node1, const shared_ptr<ruSceneNode> node2) {
	string name1 = std::const_pointer_cast<ruSceneNode>(node1)->GetName();
	string name2 = std::const_pointer_cast<ruSceneNode>(node2)->GetName();
	// find numerics in names
	int numPos1 = name1.find_first_of("0123456789");
	int numPos2 = name2.find_first_of("0123456789");
	// not found, so names doesn't contain numerics
	if (numPos1 == string::npos || numPos2 == string::npos) {
		return false;
	}
	// get num of nodes
	int num1 = atoi(name1.substr(numPos1).c_str());
	int num2 = atoi(name2.substr(numPos2).c_str());
	return num1 < num2;
}
