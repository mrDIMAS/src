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
#include "PointLight.h"

vector< PointLight* > PointLight::msPointLightList;
CubeTexture * PointLight::msDefaultPointCubeTexture;

bool PointLight::IsSeePoint( const ruVector3 & point ) {
	return (ruVector3( mGlobalTransform.getOrigin().m_floats ) - point ).Length2() < mRadius * mRadius * 4;
}

void PointLight::SetPointTexture( ruCubeTexture * cubeTexture ) {
	mPointTexture = dynamic_cast<CubeTexture*>( cubeTexture );
}

PointLight::~PointLight() {
	auto pointLight = find( msPointLightList.begin(), msPointLightList.end(), this );
	if( pointLight != msPointLightList.end() ) {
		msPointLightList.erase( pointLight );
	}
	if( Camera::msCurrentCamera ) {
		if( Camera::msCurrentCamera->mNearestPathPoint ) {
			auto & litList = Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList;
			auto iter = find( litList.begin(), litList.end(), this );
			if( iter != litList.end() ) {
				litList.erase( iter );
			}
		}		
	}
}

PointLight::PointLight() {
	mPointTexture = nullptr;
	msPointLightList.push_back( this );
	if( msDefaultPointCubeTexture ) {
		mPointTexture = msDefaultPointCubeTexture;
	}
}

ruPointLight * ruPointLight::Create() {
	return new PointLight;
}

int ruPointLight::GetCount() {
	return PointLight::msPointLightList.size();
}

ruPointLight * ruPointLight::Get( int n ) {
	return PointLight::msPointLightList[n];
}

void ruPointLight::SetPointDefaultTexture( ruCubeTexture * defaultPointTexture ) {
	PointLight::msDefaultPointCubeTexture = dynamic_cast<CubeTexture*>( defaultPointTexture );
}