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
    D3DXMATRIX depthHackMatrix;
    D3DXPLANE frustumPlanes[ 6 ];
    bool inDepthHack;
    Skybox * skybox;

public:

    explicit Camera( float fov );
    virtual ~Camera();
    void BuildFrustum();
    void CalculateProjectionMatrix();
    void CalculateInverseViewProjection();
    void SetSkyBox( const char * path );
    void Update();
    void EnterDepthHack( float depth );
    void LeaveDepthHack( );
    static Camera * CastHandle( NodeHandle handle );
};