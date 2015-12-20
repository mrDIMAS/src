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

class Light : public virtual ruLight, public SceneNode {
public:
	COMPtr<IDirect3DQuery9> pQuery;
	bool mQueryDone;
	bool mInFrustum;   
    float mRadius;
    ruVector3 mColor;
	float mGreyScaleFactor;
    static vector<Light*> msLightList;
public:
	virtual void OnLostDevice();
	virtual void OnResetDevice();
    explicit Light();
    virtual ~Light();

    virtual void SetColor( const ruVector3 & theColor );
    virtual ruVector3 GetColor() const;

    virtual void SetRange( float radius );
    virtual float GetRange() const;

	virtual void SetGreyscaleFactor( float greyScaleFactor );
	virtual float GetGrayscaleFactor() const;

	virtual bool IsSeePoint( const ruVector3 & point ) = 0;	
};