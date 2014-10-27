#pragma once

#include "Common.h"
#include "Vertex.h"

class AABB {
private:
    void CalculateVerticesRadiusCenter();
public:
    Vector3 vertices[ 8 ];
    Vector3 min;
    Vector3 max;
    Vector3 center;
    float radius;
    explicit AABB( );
    explicit AABB( const Vector3 & min, const Vector3 & max );
    explicit AABB( const vector< Vector3 > & points );
    explicit AABB( const vector< Vertex > & vertices );
    bool IsPointInside( const Vector3 & v );
    bool IsTriangleInside( const Vector3 & v1, const Vector3 & v2, const Vector3 & v3 );
    bool IsIntersects( const AABB & aabb );
    void Visualize( Vector3 offset );
};