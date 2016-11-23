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

#include "Light.h"
#include "CubeTexture.h"

class PointLight : public Light, public virtual IPointLight {
private:
	friend class SceneFactory;
	shared_ptr<CubeTexture> mPointTexture;
public:
	PointLight(SceneFactory * factory);
	~PointLight();	
	shared_ptr<CubeTexture> GetPointTexture();

	// API Methods
	virtual bool IsSeePoint(const Vector3 & point) override final;
	virtual void SetPointTexture(const shared_ptr<ICubeTexture> & cubeTexture) override final;
};