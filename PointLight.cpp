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

bool PointLight::IsSeePoint(const Vector3 & point) {
	return (Vector3(mGlobalTransform.getOrigin().m_floats) - point).Length2() < mRadius * mRadius * 4;
}

void PointLight::SetPointTexture(const shared_ptr<ICubeTexture> & cubeTexture) {
	mPointTexture = dynamic_pointer_cast<CubeTexture>(cubeTexture);
}

PointLight::~PointLight() {

}

PointLight::PointLight(SceneFactory * factory) : Light(factory) {
	mPointTexture = dynamic_pointer_cast<CubeTexture>(factory->GetPointLightDefaultTexture());	
}

shared_ptr<CubeTexture> PointLight::GetPointTexture() {
	return mPointTexture;
}
