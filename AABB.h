#pragma once

#include "Common.h"
#include "Vertex.h"

class AABB
{
private:
    void CalculateVerticesRadiusCenter();
public:
    ruVector3 vertices[ 8 ];
    ruVector3 min;
    ruVector3 max;
    ruVector3 center;
    float radius;
    explicit AABB( );
    explicit AABB( const ruVector3 & min, const ruVector3 & max );
    explicit AABB( const vector< ruVector3 > & points );
    explicit AABB( const vector< Vertex > & vertices );
    bool IsPointInside( const ruVector3 & v );
    bool IsTriangleInside( const ruVector3 & v1, const ruVector3 & v2, const ruVector3 & v3 );
    bool IsIntersects( const AABB & aabb );
    void Visualize( ruVector3 offset );
};