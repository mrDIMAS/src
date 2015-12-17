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

#pragma once

#include "SceneNode.h"
#include "Frustum.h"
#include "Texture.h"

class Skybox;
class PointLight;

class Camera : public virtual ruCamera, public SceneNode {
public:  
	float mFov;
    float mNearZ;
    float mFarZ;

    D3DXMATRIX mProjection;
    D3DXMATRIX mView;
    D3DXMATRIX mViewProjection;
    D3DXMATRIX invViewProjection;
    D3DXMATRIX mDepthHackMatrix;
    Frustum mFrustum;
    bool mInDepthHack;
    shared_ptr<Skybox> mSkybox;

	// dynamic light caching
	class PathPoint {
	public:
		ruVector3 mPoint;
		// list of lights visible from that point, filled by occlusion queries
		vector<PointLight*> mVisibleLightList;
	};
	PathPoint * mDefaultPathPoint;
	PathPoint * mNearestPathPoint;
	ruVector3 mLastPosition;
	vector<PathPoint*> mPath;
	float mPathNewPointDelta;
	void ManagePath();	
public:
	static Camera * msCurrentCamera;
    explicit Camera( float fov );
    virtual ~Camera();
    void CalculateProjectionMatrix();
    void CalculateInverseViewProjection();
    void Update();
    void EnterDepthHack( float depth );
    void LeaveDepthHack( );
	virtual void OnLostDevice();
	virtual void OnResetDevice();

	virtual void SetFOV( float fov );

	virtual void SetActive();

	virtual void SetSkybox( shared_ptr<ruTexture> up, shared_ptr<ruTexture> left, shared_ptr<ruTexture> right, shared_ptr<ruTexture> forward, shared_ptr<ruTexture> back );

};