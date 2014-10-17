#pragma once

#include "SceneNode.h"

class Skybox;

class Camera : public SceneNode {
public:
    float fov;
    float nearZ;
    float farZ;

    D3DXMATRIX projection;
    D3DXMATRIX view;
    D3DXMATRIX viewProjection;
    D3DXMATRIX invViewProjection;

    D3DXPLANE frustumPlanes[ 6 ];

    Skybox * skybox;

public:

    explicit Camera( float fov );
    virtual ~Camera();
    void BuildFrustum();
    void CalculateProjectionMatrix();
    void CalculateInverseViewProjection();
    void SetSkyBox( const char * path );
    void Update();

    static Camera * CastHandle( NodeHandle handle );
};