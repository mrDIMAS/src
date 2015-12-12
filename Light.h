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
#include "Texture.h"
#include "Camera.h"
#include "CubeTexture.h"
#include "Mesh.h"
#include "AABB.h"
#include "Frustum.h"
#include "Utility.h"

class Light : public SceneNode {
public:
	IDirect3DQuery9 * pQuery;
	bool mQueryDone;
	bool mInFrustum;
    Frustum mFrustum;
    float mRadius;
    ruVector3 mColor;
    float mInnerAngle;
    float mOuterAngle;
    float mCosHalfInnerAngle;
    float mCosHalfOuterAngle;
    ruLight::Type mType;
	float mGreyScaleFactor;
    D3DXMATRIX mSpotViewProjectionMatrix;
    Texture * mSpotTexture;
    CubeTexture * mPointTexture;

    static vector<Light*> msLightList;
    static Texture * msDefaultSpotTexture;
    static CubeTexture * msDefaultPointCubeTexture;
	static vector< Light* > msPointLightList;
	static vector< Light* > msSpotLightList; 
public:
	virtual void OnLostDevice();
	virtual void OnResetDevice();
    explicit Light( ruLight::Type type );
    virtual ~Light();
    void SetColor( const ruVector3 & theColor );
    ruVector3 GetColor() const;
    void SetRange( const float & theRadius );
    float GetRange() const;
    void SetConeAngles( float theInner, float theOuter );
    float GetInnerAngle() const;
	float GetOuterAngle() const;
    float GetCosHalfInnerAngle( );
    float GetCosHalfOuterAngle( );
    void SetSpotTexture( Texture * tex );
    void SetPointTexture( CubeTexture * ctex );
    void BuildSpotProjectionMatrixAndFrustum();
    static Light * GetLightByHandle( ruSceneNode handle );
	float GetGreyScaleFactor();
	bool IsSeePoint( const ruVector3 & point );
	void SetGreyScaleFactor( float greyScaleFactor );
};