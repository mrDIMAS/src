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
#include "SpotLight.h"
#include "SceneFactory.h"

bool SpotLight::IsSeePoint(const Vector3 & point) {
	bool inFrustum = mFrustum.IsPointInside(point);
	if(inFrustum) {
		return (Vector3(mGlobalTransform.getOrigin().m_floats) - point).Length2() < mRadius * mRadius * 4;
	}
	return false;
}

void SpotLight::BuildSpotProjectionMatrixAndFrustum() {
	Vector3 position = GetPosition();
	btVector3 bEye = btVector3(position.x, position.y, position.z);
	btVector3 bLookAt = bEye + mGlobalTransform.getBasis() * btVector3(0, -1, 0);
	btVector3 bUp = mGlobalTransform.getBasis() * btVector3(1, 0, 0);

	D3DXVECTOR3 dxEye(bEye.x(), bEye.y(), bEye.z());
	D3DXVECTOR3 dxLookAt(bLookAt.x(), bLookAt.y(), bLookAt.z());
	D3DXVECTOR3 dxUp(bUp.x(), bUp.y(), bUp.z());

	D3DXMATRIX mView, mProj;
	D3DXMatrixLookAtRH(&mView, &dxEye, &dxLookAt, &dxUp);
	D3DXMatrixPerspectiveFovRH(&mProj, mOuterAngle * SIMD_PI / 180.0f, 1.0f, 0.1f, 1000.0f);
	D3DXMatrixMultiply(&mSpotViewProjectionMatrix, &mView, &mProj);
	mFrustum.Build(mSpotViewProjectionMatrix);
}

void SpotLight::SetSpotTexture(shared_ptr<ITexture> texture) {
	mSpotTexture = std::dynamic_pointer_cast<Texture>(texture);
}

void SpotLight::SetConeAngles(float theInner, float theOuter) {
	mInnerAngle = theInner;
	mOuterAngle = theOuter;

	mCosHalfInnerAngle = cosf((mInnerAngle / 2) * SIMD_PI / 180.0f);
	mCosHalfOuterAngle = cosf((mOuterAngle / 2) * SIMD_PI / 180.0f);
}

float SpotLight::GetCosHalfOuterAngle() {
	return mCosHalfOuterAngle;
}

float SpotLight::GetCosHalfInnerAngle() {
	return mCosHalfInnerAngle;
}

float SpotLight::GetOuterAngle() const {
	return mOuterAngle;
}

float SpotLight::GetInnerAngle() const {
	return mInnerAngle;
}

SpotLight::~SpotLight() {

}

SpotLight::SpotLight(SceneFactory * factory) : Light(factory) {
	mSpotTexture = dynamic_pointer_cast<Texture>(factory->GetSpotLightDefaultTexture());
	SetConeAngles(45.0f, 80.0f);
}

Frustum & SpotLight::GetFrustum() {
	return mFrustum;
}

shared_ptr<Texture> SpotLight::GetSpotTexture() {
	return mSpotTexture;
}

D3DXMATRIX SpotLight::GetViewProjectionMatrix() {
	return mSpotViewProjectionMatrix;
}
