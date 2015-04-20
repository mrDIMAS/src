#include "Precompiled.h"

#include "AABB.h"

bool AABB::IsTriangleInside( const ruVector3 & v1, const ruVector3 & v2, const ruVector3 & v3 ) {
    return IsPointInside( v1 ) || IsPointInside( v2 ) || IsPointInside( v3 );
}

bool AABB::IsPointInside( const ruVector3 & v ) {
    return  v.x >= mMin.x && v.x <= mMax.x &&
            v.y >= mMin.y && v.y <= mMax.y &&
            v.z >= mMin.z && v.z <= mMax.z ;
}

AABB::AABB( const ruVector3 & min, const ruVector3 & max ) : mMin( min ),
															 mMax( max )
{
    mCenter = ( mMin + mMax ) / 2;
}

AABB::AABB() {

}

AABB::AABB( const vector< Vertex > & vertices ) {
    mMax = ruVector3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
    mMin = ruVector3( FLT_MAX, FLT_MAX, FLT_MAX );
    for( auto & vert : vertices ) {
        auto & v = vert.coords;
		if( v.x > mMax.x ) {
			mMax.x = v.x;
		}
		if( v.x < mMin.x ) {
			mMin.x = v.x;
		}
		if( v.y > mMax.y ) {
			mMax.y = v.y;
		} 
		if( v.y < mMin.y ) {
			mMin.y = v.y;
		}
		if( v.z > mMax.z ) {
			mMax.z = v.z;
		} 
		if( v.z < mMin.z ) {
			mMin.z = v.z;
		}
    }
	
	mCenter = ( mMin + mMax ) / 2;

	// find max metrics
	float maxSize = 0;
	ruVector3 size = mMax - mMin;
	if( size.x > maxSize ) {
		maxSize = size.x;
	} 
	if ( size.y > maxSize ) {
		maxSize = size.y;
	} 
	if ( size.z > maxSize ) {
		maxSize = size.z;
	}

	// make cube from aabb
	ruVector3 halfSize( maxSize / 2, maxSize / 2, maxSize / 2 );
	mFrustumMin = mCenter - halfSize;
	mFrustumMax = mCenter + halfSize;
}
