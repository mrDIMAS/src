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
#include "Light.h"
#include "SceneNode.h"
#include "Utility.h"

vector<Light*> Light::msLightList;

Light::Light() : mGreyScaleFactor( 0.0f ) {
    mColor = ruVector3( 1.0f, 1.0f, 1.0f );
    mRadius = 1.0f;    
	mQueryDone = true;
	OnResetDevice();
	mInFrustum = false;
}

void Light::SetColor( const ruVector3 & theColor ) {
    mColor.x = theColor.x / 255.0f;
    mColor.y = theColor.y / 255.0f;
    mColor.z = theColor.z / 255.0f;
}

ruVector3 Light::GetColor() const {
    return mColor;
}

void Light::SetRange( float theRadius )  {
    mRadius = theRadius;
}

float Light::GetRange() const {
    return mRadius;
}

Light::~Light() {
	pQuery.Reset();
}

void Light::OnResetDevice() {
	Engine::I().GetDevice()->CreateQuery( D3DQUERYTYPE_OCCLUSION, &pQuery );
	mQueryDone = true;
}

void Light::OnLostDevice() {
	pQuery->Release();
}

void Light::SetGreyscaleFactor( float greyScaleFactor ) {
	mGreyScaleFactor = greyScaleFactor;
}

float Light::GetGrayscaleFactor() const {
	return mGreyScaleFactor;
}

ruLight::~ruLight() {

}