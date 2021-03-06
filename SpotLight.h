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

class SpotLight : public virtual ISpotLight, public Light {
private:
	friend class SceneFactory;


	float mInnerAngle;
	float mOuterAngle;
	float mCosHalfInnerAngle;
	float mCosHalfOuterAngle;

	Frustum mFrustum;
	shared_ptr<Texture> mSpotTexture;
	D3DXMATRIX mSpotViewProjectionMatrix;
public:

	SpotLight(SceneFactory * factory);
	~SpotLight();
	float GetInnerAngle() const;
	float GetOuterAngle() const;
	float GetCosHalfInnerAngle();
	float GetCosHalfOuterAngle();
	void SetConeAngles(float theInner, float theOuter);
	void SetSpotTexture(shared_ptr<ITexture> texture);
	void BuildSpotProjectionMatrixAndFrustum();
	D3DXMATRIX GetViewProjectionMatrix();
	shared_ptr<Texture> GetSpotTexture();
	Frustum & GetFrustum();

	// API Methods
	virtual bool IsSeePoint(const Vector3 & point) override final;
};