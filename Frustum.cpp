#include "Precompiled.h"

#include "Frustum.h"

bool Frustum::IsAABBInside( const AABB & aabb, const ruVector3 & aabbOffset ) {
	const ruVector3 & aabbCenter = aabbOffset + (aabb.mFrustumMax + aabb.mFrustumMin) / 2;
	const ruVector3 & aabbSize = ( aabb.mFrustumMax - aabb.mFrustumMin ) / 2;
	for( unsigned int iPlane = 0; iPlane < 6; iPlane++ ) {
		const D3DXPLANE & frustumPlane = planes[iPlane];
		float d = aabbCenter.x * frustumPlane.a + aabbCenter.y * frustumPlane.b + aabbCenter.z * frustumPlane.c;
		float r = aabbSize.x * fabsf(frustumPlane.a) + aabbSize.y * fabsf(frustumPlane.b) + aabbSize.z * fabsf(frustumPlane.c);
		if( (d + r) < -frustumPlane.d)	{
			return false;
		} else if( (d - r) < -frustumPlane.d) {
			return true;
		}
	}
	return true;
}

bool Frustum::IsPointInside( const ruVector3 & point ) {
    for( int i = 0; i < 6; i++ ) {
        D3DXVECTOR3 dxPoint = D3DXVECTOR3( point.x, point.y, point.z );
        if( D3DXPlaneDotCoord( &planes[i], &dxPoint ) <= 0 ) {
            return false;
        }
    }
    return true;
}

void Frustum::Build( D3DXMATRIX viewProjection ) {
    // Left plane
    planes[0].a = viewProjection._14 + viewProjection._11;
    planes[0].b = viewProjection._24 + viewProjection._21;
    planes[0].c = viewProjection._34 + viewProjection._31;
    planes[0].d = viewProjection._44 + viewProjection._41;

    // Right plane
    planes[1].a = viewProjection._14 - viewProjection._11;
    planes[1].b = viewProjection._24 - viewProjection._21;
    planes[1].c = viewProjection._34 - viewProjection._31;
    planes[1].d = viewProjection._44 - viewProjection._41;

    // Top plane
    planes[2].a = viewProjection._14 - viewProjection._12;
    planes[2].b = viewProjection._24 - viewProjection._22;
    planes[2].c = viewProjection._34 - viewProjection._32;
    planes[2].d = viewProjection._44 - viewProjection._42;

    // Bottom plane
    planes[3].a = viewProjection._14 + viewProjection._12;
    planes[3].b = viewProjection._24 + viewProjection._22;
    planes[3].c = viewProjection._34 + viewProjection._32;
    planes[3].d = viewProjection._44 + viewProjection._42;

    // Near plane
    planes[4].a = viewProjection._13;
    planes[4].b = viewProjection._23;
    planes[4].c = viewProjection._33;
    planes[4].d = viewProjection._43;

    // Far plane
    planes[5].a = viewProjection._14 - viewProjection._13;
    planes[5].b = viewProjection._24 - viewProjection._23;
    planes[5].c = viewProjection._34 - viewProjection._33;
    planes[5].d = viewProjection._44 - viewProjection._43;

    // Normalize planes
    for ( int i = 0; i < 6; i++ ) {
        D3DXPlaneNormalize ( &planes[i], &planes[i] );
    }
}

Frustum::Frustum() {

}

bool Frustum::IsSphereInside( const ruVector3 & center, const float & radius ) {
	float fDistance;
	for(int i = 0; i < 6; ++i) {
		D3DXVECTOR4 center( center.x, center.y, center.z, 1.0f );
		fDistance = D3DXPlaneDot( &planes[i], &center );
		if(fDistance < -radius) {
			return false;
		}
		if( fabs( fDistance ) < radius ) {
			return true;
		}
	}
	return true;
}
