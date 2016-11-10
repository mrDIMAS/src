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
#include "PointLight.h"
#include "SceneFactory.h"

CubeTexture * PointLight::msDefaultPointCubeTexture;

bool PointLight::IsSeePoint( const ruVector3 & point ) {
	return (ruVector3( mGlobalTransform.getOrigin().m_floats ) - point ).Length2() < mRadius * mRadius * 4;
}

void PointLight::SetPointTexture( ruCubeTexture * cubeTexture ) {
	mPointTexture = dynamic_cast<CubeTexture*>( cubeTexture );
}

PointLight::~PointLight() {

}

PointLight::PointLight() {
	mPointTexture = nullptr;
	if( msDefaultPointCubeTexture ) {
		mPointTexture = msDefaultPointCubeTexture;
	}
}

CubeTexture * PointLight::GetPointTexture()
{
	return mPointTexture;
}

shared_ptr<ruPointLight> ruPointLight::Create() {
	return SceneFactory::CreatePointLight();
}

int ruPointLight::GetCount() {
	return SceneFactory::GetPointLightList().size();
}

shared_ptr<ruPointLight> ruPointLight::Get( int n ) {
	return SceneFactory::GetPointLightList()[n].lock();
}

void ruPointLight::SetPointDefaultTexture( ruCubeTexture * defaultPointTexture ) {
	PointLight::msDefaultPointCubeTexture = dynamic_cast<CubeTexture*>( defaultPointTexture );
}