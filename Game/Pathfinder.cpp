#include "Pathfinder.h"

void Pathfinder::BuildPath( GraphVertex * begin, GraphVertex * end, vector< GraphVertex* > & outPoints ) {
    assert( begin != nullptr );
    assert( end != nullptr );
    assert( graph.size() != 0 );
    outPoints.clear();
    // clear state of all vertices
    for( auto vertex : graph ) {
        vertex->ClearState();
    }
    // set begin graph vertex
    begin->distanceFromBegin = 0;

    for( int i = 0; i < graph.size(); i++ ) {
        // get nearest vertex
        GraphVertex * nearest = nullptr;
        for( auto vertex : graph ) {
            if( vertex->used ) {
                continue;
            } else if( !nearest ) {
                nearest = vertex;
            } else if( vertex->distanceFromBegin < nearest->distanceFromBegin ) {
                nearest = vertex;
            }
        }

        if( nearest->distanceFromBegin >= Infinite ) {
            break;
        }

        nearest->used = true;

        // relaxation
        for( auto & edge : nearest->edges ) {
            if( nearest->distanceFromBegin + edge.distToDestVertex < edge.destVertex->distanceFromBegin ) {
                edge.destVertex->distanceFromBegin = nearest->distanceFromBegin + edge.distToDestVertex;
                edge.destVertex->ancestor = nearest;
            }
        }
    }

    // restore path to dest vertex
    for( GraphVertex * v = end; v != begin; v = v->ancestor ) {
        outPoints.push_back (v);
    }

    outPoints.push_back( begin );

    reverse( outPoints.begin(), outPoints.end() );
}

int Pathfinder::GetPointCount() {
    return graph.size();
}

GraphVertex * Pathfinder::GetPoint( int i ) {
    if( i < 0 || i >= graph.size() ) {
        return nullptr;
    } else {
        return graph[i];
    }
}

void Pathfinder::SetVertices( vector< GraphVertex* > vertices ) {
    graph = vertices;
}

Pathfinder::~Pathfinder() {
    for( auto vertex : graph ) {
        delete vertex;
    }
}

Pathfinder::Pathfinder() {

}

GraphVertex * Pathfinder::GetVertexNearestTo( ruVector3 position, int * vertexNum ) {
    assert( graph.size() != 0 );
    GraphVertex * nearest = graph[0];
    int nearestIndex = 0, index = 0;;
    for( auto vertex : graph ) {
        if( ( vertex->position - position ).Length2() < ( nearest->position - position ).Length2() ) {
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
    edges.push_back( Edge( vertex, DistanceToVertex( vertex )));
    vertex->edges.push_back( Edge( this, DistanceToVertex( vertex )));
}

void GraphVertex::ClearState() {
    ancestor = nullptr;
    used = false;
    distanceFromBegin = Infinite;
}

GraphVertex::GraphVertex( ruVector3 pos ) {
    position = pos;
    ClearState();
}

float GraphVertex::DistanceToVertex( GraphVertex * vertex ) {
    return ( position - vertex->position ).Length();
}

Edge::Edge( GraphVertex * destinationVertex, float distanceToDestinationVertex ) {
    destVertex = destinationVertex;
    distToDestVertex = distanceToDestinationVertex;
}

Edge::Edge() {
    destVertex = nullptr;
    distToDestVertex = Infinite;
}

void Path::BuildPath( ruNodeHandle scene, string pathBaseName ) {
    vector<ruNodeHandle> pointNodes;
    for( int i = 0; i < ruGetNodeCountChildren( scene ); i++ ) {
        ruNodeHandle child = ruGetNodeChild( scene, i );
        string cName = ruGetNodeName( child );
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
        AddPointAndLinkWithPrevious( new GraphVertex( ruGetNodePosition( node ) ) );
    }
}

void Path::AddPointAndLinkWithPrevious( GraphVertex * vertex ) {
    if( vertices.size() > 0 ) {
        GraphVertex * prev = vertices[ vertices.size() - 1 ];
        prev->AddEdge( vertex );
    }
    vertices.push_back( vertex );
}

bool Path::NodeSorter::operator()( const ruNodeHandle & node1, const ruNodeHandle & node2 ) {
    string name1 = ruGetNodeName( node1 );
    string name2 = ruGetNodeName( node2 );
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
