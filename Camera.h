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

#pragma once

#include "SceneNode.h"
#include "Frustum.h"
#include "Texture.h"

class Skybox;
class PointLight;

// to efficienty light management (exclude invisible lights from render chain) we create path of a camera 
// that contains special points. These points contains list of light, that can 'be seen' from this point.
// List of visible lights is filled by occlusion queries inside the render chain 
class PathPoint {
private:
	Vector3 mPoint;
	// list of lights visible from that point, filled by occlusion queries
	vector<weak_ptr<PointLight>> mVisibleLightList;
public:
	Vector3 GetPosition() const {
		return mPoint;
	}
	explicit PathPoint(const Vector3 & position) : mPoint(position) {

	}
	template<typename Type>
	void RemoveUnreferenced(vector<weak_ptr<Type>>& objList) {
		for(auto iter = objList.begin(); iter != objList.end(); ) {
			if((*iter).use_count()) {
				++iter;
			} else {
				iter = objList.erase(iter);
			}
		}
	}
	vector<weak_ptr<PointLight>> & GetListOfVisibleLights() {
		RemoveUnreferenced(mVisibleLightList);
		return mVisibleLightList;
	}
};

class Camera : public virtual ICamera, public SceneNode {
private:
	friend class SceneFactory;

public:
	float mFov;
	float mNearZ;
	float mFarZ;
	float mFrameBrightness;
	Vector3 mFrameColor;

	D3DXMATRIX mProjection;
	D3DXMATRIX mView;
	D3DXMATRIX mViewProjection;
	D3DXMATRIX mInverseView;
	D3DXMATRIX invViewProjection;
	D3DXMATRIX mDepthHackMatrix;
	Frustum mFrustum;
	bool mInDepthHack;
	shared_ptr<Skybox> mSkybox;
	int mNearestPathPointIndex;
	Vector3 mLastPosition;
	vector<unique_ptr<PathPoint>> mPath;
	float mPathNewPointDelta;
	void ManagePath();
public:
	explicit Camera(SceneFactory * factory, float fov);
	static weak_ptr<Camera> msCurrentCamera;
	virtual ~Camera();
	void CalculateProjectionMatrix();
	void CalculateInverseViewProjection();
	void Update();
	// Modify projection matrix to add depth offset, applied only of depth > 0.001
	void EnterDepthHack(float depth);
	void LeaveDepthHack();
	unique_ptr<PathPoint> & GetNearestPathPoint();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
	int GetPathSize() const {
		return mPath.size();
	}
	// API Methods
	virtual void SetFOV(float fov) override;
	virtual void SetActive() override;
	virtual void SetSkybox(const shared_ptr<ITexture> & up, const shared_ptr<ITexture> & left, const shared_ptr<ITexture> & right, const shared_ptr<ITexture> & forward, const shared_ptr<ITexture> & back) override;
	virtual void SetFrameBrightness(float brightness) override;
	virtual float GetFrameBrightness() const override;
	virtual void SetFrameColor(const Vector3 & color) override;
	virtual Vector3 GetFrameColor() const override;
};