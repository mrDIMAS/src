#include "Precompiled.h"

#include "Pathfinder.h"

void Pathfinder::BuildPath( GraphVertex * begin, GraphVertex * end, vector< GraphVertex* > & outPoints ) {
	// this is modified Dijkstra path-finding algorithm
    assert( begin != nullptr );
    assert( end != nullptr );
    assert( mGraph.size() != 0 );
    outPoints.clear();
    // clear state of all vertices
    for( auto vertex : mGraph ) {
        vertex->ClearState();
    }
    // set begin graph vertex
    begin->mDistanceFromBegin = 0;

    for( int i = 0; i < mGraph.size(); i++ ) {
        // get nearest vertex
        GraphVertex * nearest = nullptr;
        for( auto vertex : mGraph ) {
            if( vertex->mUsed ) {
                continue;
            } else if( !nearest ) {
                nearest = vertex;
            } else if( vertex->mDistanceFromBegin < nearest->mDistanceFromBegin ) {
                nearest = vertex;
            }
        }

        if( nearest->mDistanceFromBegin >= Infinite ) {
            break;
        }

        nearest->mUsed = true;

        // relaxation
        for( auto & edge : nearest->mEdges ) {
            if( nearest->mDistanceFromBegin + edge.mDistToDestVertex < edge.mpDestVertex->mDistanceFromBegin ) {
                edge.mpDestVertex->mDistanceFromBegin = nearest->mDistanceFromBegin + edge.mDistToDestVertex;
                edge.mpDestVertex->mAncestor = nearest;
            }
        }
    }

    // restore path to dest vertex
    for( GraphVertex * v = end; v != begin; v = v->mAncestor ) {
        outPoints.push_back (v);
    }

    outPoints.push_back( begin );

    reverse( outPoints.begin(), outPoints.end() );
}

int Pathfinder::GetPointCount() {
    return mGraph.size();
}

GraphVertex * Pathfinder::GetPoint( int i ) {
    if( i < 0 || i >= mGraph.size() ) {
        return nullptr;
    } else {
        return mGraph[i];
    }
}

void Pathfinder::SetVertices( vector< GraphVertex* > vertices ) {
    mGraph = vertices;
}

Pathfinder::~Pathfinder() {
    for( auto vertex : mGraph ) {
        delete vertex;
    }
}

Pathfinder::Pathfinder() {

}

GraphVertex * Pathfinder::GetVertexNearestTo( ruVector3 position, int * vertexNum ) {
    assert( mGraph.size() != 0 );
    GraphVertex * nearest = mGraph[0];
    int nearestIndex = 0, index = 0;;
    for( auto vertex : mGraph ) {
        if( ( vertex->mPosition - position ).Length2() < ( nearest->mPosition - position ).Length2() ) {
            nearest = vertex;
            nearestIndex = index;
        }
        index++;
    }
    if( vertexNum ) {
        *vertexNum = nearestIndex;
    }
    return nearest;
}

void GraphVertex::AddEdge( GraphVertex * vertex ) {
    mEdges.push_back( Edge( vertex, DistanceToVertex( vertex )));
    vertex->mEdges.push_back( Edge( this, DistanceToVertex( vertex )));
}

void GraphVertex::ClearState() {
    mAncestor = nullptr;
    mUsed = false;
    mDistanceFromBegin = Infinite;
}

GraphVertex::GraphVertex( ruVector3 pos ) {
    mPosition = pos;
    ClearState();
}

float GraphVertex::DistanceToVertex( GraphVertex * vertex ) {
    return ( mPosition - vertex->mPosition ).Length();
}

Edge::Edge( GraphVertex * destinationVertex, float distanceToDestinationVertex ) {
    mpDestVertex = destinationVertex;
    mDistToDestVertex = distanceToDestinationVertex;
}

Edge::Edge() {
    mpDestVertex = nullptr;
    mDistToDestVertex = Infinite;
}

void Path::BuildPath( shared_ptr<ruSceneNode> scene, string pathBaseName ) {
    vector<shared_ptr<ruSceneNode>> pointNodes;
    for( int i = 0; i < scene->GetCountChildren(); i++ ) {
        shared_ptr<ruSceneNode> child = scene->GetChild( i );
        string cName = child->GetName();
        if( cName.size() < pathBaseName.size() ) {
            continue;
        }
        int numPos = cName.find_first_of( "0123456789" );
        if( numPos > pathBaseName.size() ) {
            continue;
        }
        string bName = cName.substr( 0, pathBaseName.size() );
        if( bName == pathBaseName ) {
            pointNodes.push_back( child );
        }
    }
    NodeSorter nodeSorter;
    sort( pointNodes.begin(), pointNodes.end(), nodeSorter );
    for( auto node : pointNodes ) {
        AddPointAndLinkWithPrevious( new GraphVertex( node->GetPosition() ) );
    }
}

void Path::AddPointAndLinkWithPrevious( GraphVertex * vertex ) {
    if( mVertexList.size() > 0 ) {
        GraphVertex * prev = mVertexList[ mVertexList.size() - 1 ];
        prev->AddEdge( vertex );
    }
    mVertexList.push_back( vertex );
}

bool Path::NodeSorter::operator()( const shared_ptr<ruSceneNode> node1, const shared_ptr<ruSceneNode> node2 ) {
    string name1 = std::const_pointer_cast<ruSceneNode>(node1)->GetName();
    string name2 = std::const_pointer_cast<ruSceneNode>(node2)->GetName();
    // find numerics in names
    int numPos1 = name1.find_first_of( "0123456789" );
    int numPos2 = name2.find_first_of( "0123456789" );
    // not found, so names doesn't contain numerics
    if( numPos1 == string::npos || numPos2 == string::npos ) {
        return false;
    }
    // get num of nodes
    int num1 = atoi( name1.substr( numPos1 ).c_str() );
    int num2 = atoi( name2.substr( numPos2 ).c_str() );
    return num1 < num2;
}
