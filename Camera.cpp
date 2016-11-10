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
#include "Renderer.h"
#include "SceneNode.h"
#include "AABB.h"
#include "Camera.h"
#include "Skybox.h"
#include "PointLight.h"
#include "SceneFactory.h"

weak_ptr<Camera> Camera::msCurrentCamera;

void Camera::Update() {
	CalculateGlobalTransform();

	btVector3 eye = mGlobalTransform.getOrigin();
	btVector3 look = eye + mGlobalTransform.getBasis().getColumn(2);
	btVector3 up = mGlobalTransform.getBasis().getColumn(1);

	btVector3 normlook = mGlobalTransform.getBasis().getColumn(2).normalized();

	float f_up[3] = { 0.0f, 1.0f, 0.0f };

	// sound listener
	pfSetListenerOrientation(normlook.m_floats, f_up);
	pfSetListenerPosition(eye.x(), eye.y(), eye.z());

	// view matrix
	D3DXVECTOR3 ep(eye.x(), eye.y(), eye.z());
	D3DXVECTOR3 lv(look.x(), look.y(), look.z());
	D3DXVECTOR3 uv(up.x(), up.y(), up.z());
	D3DXMatrixLookAtRH(&mView, &ep, &lv, &uv);

	D3DXMatrixInverse(&mInverseView, nullptr, &mView);

	CalculateProjectionMatrix();
	CalculateInverseViewProjection();

	mFrustum.Build(mViewProjection);

	ManagePath();
}

void Camera::SetSkybox(const shared_ptr<ruTexture> & up, const shared_ptr<ruTexture> & left, const shared_ptr<ruTexture> & right, const shared_ptr<ruTexture> & forward, const shared_ptr<ruTexture> & back) {
	if (up && left && right && forward && back) {
		mSkybox = shared_ptr<Skybox>(new Skybox(std::dynamic_pointer_cast<Texture>(up), std::dynamic_pointer_cast<Texture>(left), std::dynamic_pointer_cast<Texture>(right), std::dynamic_pointer_cast<Texture>(forward), std::dynamic_pointer_cast<Texture>(back)));
	} else {
		if (mSkybox) {
			mSkybox.reset();
		}
	}
}

void Camera::SetFrameBrightness(float brightness) {
	mFrameBrightness = brightness;
	if (mFrameBrightness > 100.0f) {
		mFrameBrightness = 100.0f;
	}
	if (mFrameBrightness < 0.0f) {
		mFrameBrightness = 0.0f;
	}
}

float Camera::GetFrameBrightness() const {
	return mFrameBrightness;
}

void Camera::SetFrameColor(const ruVector3 & color) {
	mFrameColor = color;

	if (mFrameColor.x > 255.0f) {
		mFrameColor.x = 255.0f;
	}
	if (mFrameColor.y > 255.0f) {
		mFrameColor.y = 255.0f;
	}
	if (mFrameColor.z > 255.0f) {
		mFrameColor.z = 255.0f;
	}

	if (mFrameColor.x < 0.0f) {
		mFrameColor.x = 0.0f;
	}
	if (mFrameColor.y < 0.0f) {
		mFrameColor.y = 0.0f;
	}
	if (mFrameColor.z < 0.0f) {
		mFrameColor.z = 0.0f;
	}
}

ruVector3 Camera::GetFrameColor() const {
	return mFrameColor;
}

void Camera::CalculateInverseViewProjection() {
	D3DXMatrixMultiply(&mViewProjection, &mView, &mProjection);
	D3DXMatrixInverse(&invViewProjection, 0, &mViewProjection);
}

void Camera::CalculateProjectionMatrix() {
	D3DVIEWPORT9 vp;
	pD3D->GetViewport(&vp);
	D3DXMatrixPerspectiveFovRH(&mProjection, mFov * 3.14159 / 180.0f, (float)vp.Width / (float)vp.Height, mNearZ, mFarZ);
}

Camera::~Camera() {

}

Camera::Camera(float fov) :
	mFrameBrightness(100.0f),
	mNearZ(0.025f),
	mFarZ(6000.0f),
	mFov(fov),
	mInDepthHack(false),
	mPathNewPointDelta(5.0f),
	mFrameColor(255.0f, 255.0f, 255.0f)
{
	// path must contain at least one point, add new one located in camera's position 
	mNearestPathPointIndex = 0;
	mPath.push_back(std::move(unique_ptr<PathPoint>(new PathPoint(GetPosition()))));

	CalculateProjectionMatrix();
	D3DXMatrixLookAtRH(&mView, &D3DXVECTOR3(0, 100, 100), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
}

void Camera::EnterDepthHack(float depth) {
	depth = fabs(depth);

	if (depth > 0.001) {
		if (!mInDepthHack) {
			mDepthHackMatrix = mProjection;
		}
		mInDepthHack = true;
		mProjection._43 -= depth;
		CalculateInverseViewProjection();
	}
}

void Camera::LeaveDepthHack() {
	if (mInDepthHack) {
		mInDepthHack = false;
		mProjection = mDepthHackMatrix;
		CalculateInverseViewProjection();
	}
}

// this function builds path of camera by creating points in regular distance between them
void Camera::ManagePath() {
	if (mPath.size() > 64) {
		// get half of average distance between lights
		auto & pointLights = SceneFactory::GetPointLightList();
		if (pointLights.size()) {
			float mRangeSum = 0;
			for (auto & lWeak : pointLights) {
				shared_ptr<PointLight> & light = lWeak.lock();
				if (light) {
					mRangeSum += light->GetRange();
				}
			}
			mPathNewPointDelta = (mRangeSum / pointLights.size()) / 2;
			mLastPosition = ruVector3(FLT_MAX, FLT_MAX, FLT_MAX);
		}
		mPath.clear();
	}

	// check how far we from last added point
	ruVector3 position = GetPosition();
	if ((position - mLastPosition).Length2() > mPathNewPointDelta) {
		mLastPosition = position;
		bool addNewPoint = true;
		float distToNearest = -FLT_MAX;
		// check how far we from other path points, if far enough addNewPoint sets to true
		int index = 0;
		for (auto & pPoint : mPath) {
			float dist = (position - pPoint->GetPosition()).Length2();
			if (dist < distToNearest) {
				mNearestPathPointIndex = index;
				distToNearest = dist;
			}
			if (dist < mPathNewPointDelta) {
				addNewPoint = false;
			}
			++index;
		}
		if (addNewPoint) {
			mNearestPathPointIndex = mPath.size();
			mPath.push_back(std::move(unique_ptr<PathPoint>(new PathPoint(position))));
		}
	}
}

void Camera::OnResetDevice() {
	ManagePath();
}

void Camera::OnLostDevice() {
	mPath.clear();

	auto & pointLights = SceneFactory::GetPointLightList();
	if (pointLights.size()) {
		float mRangeSum = 0;
		for (auto & pLight : pointLights) {
			shared_ptr<PointLight> & light = pLight.lock();
			if (light) {
				mRangeSum += light->GetRange();
			}
		}
		mPathNewPointDelta = (mRangeSum / pointLights.size()) / 2;
	} else {
		mPathNewPointDelta = 0.0f;
	}
	mLastPosition = ruVector3(FLT_MAX, FLT_MAX, FLT_MAX);

	// path must contain at least one point, add new one located in camera's position 
	mNearestPathPointIndex = 0;
	mPath.push_back(std::move(unique_ptr<PathPoint>(new PathPoint(GetPosition()))));
}

void Camera::SetActive() {
	Camera::msCurrentCamera = std::dynamic_pointer_cast<Camera>(shared_from_this());
}

void Camera::SetFOV(float fov) {
	mFov = fov;
}

unique_ptr<PathPoint> & Camera::GetNearestPathPoint() {
	return mPath[mNearestPathPointIndex];
}

shared_ptr<ruCamera> ruCamera::Create(float fov) {
	return std::move(SceneFactory::CreateCamera(fov));
};
