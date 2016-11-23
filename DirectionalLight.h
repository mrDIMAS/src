#pragma once

#include "Light.h"

class DirectionalLight : public virtual IDirectionalLight, public Light {
private:
	friend class SceneFactory;
public:
	DirectionalLight(SceneFactory * factory);
	~DirectionalLight( );

	D3DXMATRIX BuildViewMatrix( const shared_ptr<Camera> & camera );

	// API Methods
	virtual bool IsSeePoint( const Vector3 & point ) override final;
};

