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
#include "Frustum.h"

bool Frustum::IsAABBInside( const AABB & aabb, const Vector3 & aabbOffset, const D3DXMATRIX & worldMatrix ) {
	const float * matrix = &worldMatrix.m[ 0 ][ 0 ];

	Vector3 aabbPoints[ 8 ];

	for ( int i = 0; i < 8; ++i ) {
		Vector3 p = aabb.mPoints[ i ];
		aabbPoints[ i ].x = p.x * matrix[ 0 ] + p.y * matrix[ 4 ] + p.z * matrix[ 8 ];
		aabbPoints[ i ].y = p.x * matrix[ 1 ] + p.y * matrix[ 5 ] + p.z * matrix[ 9 ];
		aabbPoints[ i ].z = p.x * matrix[ 2 ] + p.y * matrix[ 6 ] + p.z * matrix[ 10 ];
		aabbPoints[ i ] += aabbOffset;
	}

	for ( int i = 0; i < 6; ++i ) {
		int planeBackPoints = 0;
		for ( int k = 0; k < 8; ++k ) {
			if ( mPlanes[ i ].Dot( aabbPoints[ k ] ) <= 0 ) {
				if ( ++planeBackPoints >= 8 ) {
					return false;
				}
			}
		}
	}

	return true;
}

bool Frustum::IsPointInside( const Vector3 & point ) {
	for ( int i = 0; i < 6; i++ ) {
		if ( mPlanes[ i ].Dot( point ) <= 0 ) {
			return false;
		}
	}
	return true;
}

void Frustum::Build( D3DXMATRIX vp ) {
	mPlanes[ 0 ] = Plane( vp._14 + vp._11, vp._24 + vp._21, vp._34 + vp._31, vp._44 + vp._41 ).Normalize( ); // Left plane    
	mPlanes[ 1 ] = Plane( vp._14 - vp._11, vp._24 - vp._21, vp._34 - vp._31, vp._44 - vp._41 ).Normalize( ); // Right plane    
	mPlanes[ 2 ] = Plane( vp._14 - vp._12, vp._24 - vp._22, vp._34 - vp._32, vp._44 - vp._42 ).Normalize( ); // Top plane
	mPlanes[ 3 ] = Plane( vp._14 + vp._12, vp._24 + vp._22, vp._34 + vp._32, vp._44 + vp._42 ).Normalize( ); // Bottom plane        
	mPlanes[ 4 ] = Plane( vp._14 - vp._13, vp._24 - vp._23, vp._34 - vp._33, vp._44 - vp._43 ).Normalize( ); // Far plane
	mPlanes[ 5 ] = Plane( vp._13, vp._23, vp._33, vp._43 ).Normalize( ); // Near plane
}

Frustum::Frustum( ) {

}

bool Frustum::IsSphereInside( const Vector3 & center, const float & radius ) {
	for ( int i = 0; i < 6; ++i ) {
		float fDistance = mPlanes[ i ].Dot( center );
		if ( fDistance < -radius ) {
			return false;
		}
		if ( fabs( fDistance ) < radius ) {
			return true;
		}
	}
	return true;
}
