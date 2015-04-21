#pragma once


#include "Vertex.h"

class AABB {
public:
	ruVector3 mFrustumMax;
	ruVector3 mFrustumMin;
    ruVector3 mMin;
    ruVector3 mMax;
    ruVector3 mCenter;
    explicit AABB( );
    explicit AABB( const ruVector3 & min, const ruVector3 & max );
    explicit AABB( const vector< Vertex > & vertices );
    bool IsPointInside( const ruVector3 & v );
    bool IsTriangleInside( const ruVector3 & v1, const ruVector3 & v2, const ruVector3 & v3 );
};