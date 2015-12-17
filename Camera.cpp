/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
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
#include "Engine.h"
#include "SceneNode.h"
#include "AABB.h"
#include "Camera.h"
#include "Skybox.h"
#include "PointLight.h"

Camera * Camera::msCurrentCamera = nullptr;

void Camera::Update() {
    btVector3 eye = mGlobalTransform.getOrigin();
    btVector3 look = eye + mGlobalTransform.getBasis().getColumn ( 2 );
    btVector3 up = mGlobalTransform.getBasis().getColumn ( 1 );

    btVector3 normlook = mGlobalTransform.getBasis().getColumn ( 2 ).normalized();

    float f_up[ 3 ] = { 0.0f, 1.0f, 0.0f };

    // sound listener
    pfSetListenerOrientation( normlook.m_floats, f_up );
    pfSetListenerPosition( eye.x(), eye.y(), eye.z() );

    // view matrix
    D3DXVECTOR3 ep (  eye.x(),  eye.y(),  eye.z() );
    D3DXVECTOR3 lv ( look.x(), look.y(), look.z() );
    D3DXVECTOR3 uv (   up.x(),   up.y(),   up.z() );
    D3DXMatrixLookAtRH ( &mView, &ep, &lv, &uv );

    CalculateProjectionMatrix();
    CalculateInverseViewProjection();

    mFrustum.Build( mViewProjection );

	ManagePath();
}

void Camera::SetSkybox( shared_ptr<ruTexture> up, shared_ptr<ruTexture> left, shared_ptr<ruTexture> right, shared_ptr<ruTexture> forward, shared_ptr<ruTexture> back ) {
    if( up && left && right && forward && back ) {
        mSkybox = shared_ptr<Skybox>( new Skybox( std::dynamic_pointer_cast<Texture>( up ), std::dynamic_pointer_cast<Texture>( left ), std::dynamic_pointer_cast<Texture>( right ), std::dynamic_pointer_cast<Texture>( forward ), std::dynamic_pointer_cast<Texture>( back )));
    } else {
        if( mSkybox ) {
            mSkybox.reset();
        }
    }
}

void Camera::CalculateInverseViewProjection() {
    D3DXMatrixMultiply( &mViewProjection, &mView, &mProjection );
    D3DXMatrixInverse( &invViewProjection, 0, &mViewProjection );
}

void Camera::CalculateProjectionMatrix() {
    D3DVIEWPORT9 vp;
    Engine::I().GetDevice()->GetViewport( &vp );
    D3DXMatrixPerspectiveFovRH( &mProjection, mFov * 3.14159 / 180.0f, (float)vp.Width / (float)vp.Height, mNearZ, mFarZ );
}

Camera::~Camera() {
	if( msCurrentCamera == this ) {
		msCurrentCamera = nullptr;
	}
	for( auto pPoint : mPath ) {
		delete pPoint;
	}
}

Camera::Camera( float fov ) {
    this->mFov = fov;
    mNearZ = 0.025f;
    mFarZ = 6000.0f;
    mSkybox = nullptr;
    Camera::msCurrentCamera = this;
    mInDepthHack = false;
	mPathNewPointDelta = 5.0f;
	mDefaultPathPoint = new PathPoint;
	mDefaultPathPoint->mPoint = GetPosition();
	mNearestPathPoint = mDefaultPathPoint;
	
	mPath.push_back( mNearestPathPoint );
    CalculateProjectionMatrix();
    D3DXMatrixLookAtRH( &mView, &D3DXVECTOR3( 0, 100, 100 ), &D3DXVECTOR3( 0, 0, 0), &D3DXVECTOR3( 0, 1, 0 ));
}

void Camera::EnterDepthHack( float depth ) {
    if( !mInDepthHack ) {
        mDepthHackMatrix = mProjection;
    }
    mInDepthHack = true;
    mProjection._43 -= depth;
    CalculateInverseViewProjection();
}

void Camera::LeaveDepthHack() {
    mInDepthHack = false;
    mProjection = mDepthHackMatrix;
    CalculateInverseViewProjection();
}

// this function builds path of camera by creating points in regular distance between them
void Camera::ManagePath() {
	if( mPath.size() > 64 ) {
		if( PointLight::msPointLightList.size() ) {
			for( auto pPoint : mPath ) {
				delete pPoint;
			}
			float mRangeSum = 0;
			for( auto pLight : PointLight::msPointLightList ) {
				mRangeSum += pLight->GetRange();
			}
			mPathNewPointDelta = ( mRangeSum / PointLight::msPointLightList.size() ) / 2;
			mLastPosition = ruVector3( FLT_MAX, FLT_MAX, FLT_MAX );
		}
		mPath.clear();
	}
	
	ruVector3 position = GetPosition();
	if( (position - mLastPosition).Length2() > mPathNewPointDelta ) {
		mLastPosition = position;
		bool addNewPoint = true;
		float distToNearest = -FLT_MAX;
		for( auto pPoint : mPath ) {
			float dist = (position - pPoint->mPoint ).Length2();
			if( dist < distToNearest ) {
				mNearestPathPoint = pPoint;
				distToNearest = dist;
			}
			if( dist < mPathNewPointDelta ) {				
				addNewPoint = false;
			}
		}
		if( addNewPoint ) {
			PathPoint * pathPoint = new PathPoint;
			pathPoint->mPoint = position;
			mPath.push_back( pathPoint );
			mNearestPathPoint = pathPoint;
		}
	}
}

void Camera::OnResetDevice()
{
	ManagePath();
}

void Camera::OnLostDevice() {
	mNearestPathPoint = mDefaultPathPoint;

	for( auto pPoint : mPath ) {
		delete pPoint;
	}
	if( PointLight::msPointLightList.size() ) {
		float mRangeSum = 0;
		for( auto pLight : PointLight::msPointLightList ) {
			mRangeSum += pLight->GetRange();
		}	
		mPathNewPointDelta = ( mRangeSum / PointLight::msPointLightList.size() ) / 2;
	} else {
		mPathNewPointDelta = 0.0f;
	}
	mLastPosition = ruVector3( FLT_MAX, FLT_MAX, FLT_MAX );
	mPath.clear();
}

void Camera::SetActive()
{
	Camera::msCurrentCamera = this;
}

void Camera::SetFOV( float fov )
{
	mFov = fov;
}


