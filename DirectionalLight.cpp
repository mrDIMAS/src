#include "Precompiled.h"
#include "DirectionalLight.h"
#include "SceneFactory.h"

DirectionalLight::DirectionalLight(SceneFactory * factory) : Light(factory) {

}

DirectionalLight::~DirectionalLight() {

}

D3DXMATRIX DirectionalLight::BuildViewMatrix(const shared_ptr<Camera> & camera) {
	btVector3 bLookAt = mGlobalTransform.getBasis() * btVector3(0, -1, 0);

	D3DXVECTOR3 dxEye(0, 0, 0);
	D3DXVECTOR3 dxLookAt(bLookAt.x(), bLookAt.y(), bLookAt.z());
	D3DXVECTOR3 dxUp(0, 1, 0);

	D3DXMATRIX mView;
	D3DXMatrixLookAtRH(&mView, &dxEye, &dxLookAt, &dxUp);

	return mView;
}

// API Methods

bool DirectionalLight::IsSeePoint(const Vector3 & point) {
	return true;
}
