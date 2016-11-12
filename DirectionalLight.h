#pragma once

#include "Light.h"

class DirectionalLight : public virtual ruDirectionalLight, public Light {
private:
	friend class SceneFactory;
public:
	DirectionalLight();
	~DirectionalLight();

	D3DXMATRIX BuildViewMatrix(const shared_ptr<Camera> & camera);

	// API Methods
	virtual bool IsSeePoint(const ruVector3 & point) override final;
};

