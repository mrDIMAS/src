/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#include "Precompiled.h"

#include "AABB.h"

void AABB::BuildPoints()
{
	mPoints[0] = ruVector3( mMin.x, mMin.y, mMin.z );
	mPoints[1] = ruVector3( mMin.x, mMin.y, mMax.z );
	mPoints[2] = ruVector3( mMax.x, mMin.y, mMax.z );
	mPoints[3] = ruVector3( mMax.x, mMin.y, mMin.z );
	mPoints[4] = ruVector3( mMin.x, mMax.y, mMin.z );
	mPoints[5] = ruVector3( mMin.x, mMax.y, mMax.z );
	mPoints[6] = ruVector3( mMax.x, mMax.y, mMax.z );
	mPoints[7] = ruVector3( mMax.x, mMax.y, mMin.z );
}

float AABB::Squared( float x )
{
	return x * x;
}

AABB::AABB( const ruVector3 & min, const ruVector3 & max ) : mMin( min ), mMax( max ) {
	BuildPoints();
}

AABB::AABB() {

}

AABB::AABB( const vector< Vertex > & vertices ) {
    mMax = ruVector3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
    mMin = ruVector3( FLT_MAX, FLT_MAX, FLT_MAX );

    for( auto & vert : vertices ) {
        auto & v = vert.mPosition;
		if( v.x > mMax.x ) mMax.x = v.x;
		if( v.x < mMin.x ) mMin.x = v.x;
		if( v.y > mMax.y ) mMax.y = v.y;
		if( v.y < mMin.y ) mMin.y = v.y;
		if( v.z > mMax.z ) mMax.z = v.z;
		if( v.z < mMin.z ) mMin.z = v.z;
    }

	BuildPoints();
}

bool AABB::IsIntersectSphere( const ruVector3 & aabbOffset, const ruVector3 & position, float radius )
{
	float r2 = radius * radius;
	float dmin = 0;

	ruVector3 max = mMax + aabbOffset;
	ruVector3 min = mMin + aabbOffset;

	if( position.x < min.x ) {
		dmin += Squared( position.x - min.x );
	} else if( position.x > max.x ) {
		dmin += Squared( position.x - max.x );
	}

	if( position.y < min.y ) {
		dmin += Squared( position.y - min.y );
	} else if( position.y > max.y ) {
		dmin += Squared( position.y - max.y );
	}

	if( position.z < min.z ) {
		dmin += Squared( position.z - min.z );
	} else if( position.z > max.z ) {
		dmin += Squared( position.z - max.z );
	}

	bool sphereInside = 
		(position.x >= min.x) && (position.x <= max.x) &&
		(position.y >= min.y) && (position.y <= max.y) &&
		(position.z >= min.z) && (position.z <= max.z);

	return dmin <= r2 || sphereInside;
}
