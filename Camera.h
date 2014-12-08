#pragma once

#include "SceneNode.h"
#include "Frustum.h"

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
    Frustum frustum;
    bool inDepthHack;
    Skybox * skybox;
public:
    explicit Camera( float fov );
    virtual ~Camera();
    void CalculateProjectionMatrix();
    void CalculateInverseViewProjection();
    void SetSkyBox( const char * path );
    void Update();
    void EnterDepthHack( float depth );
    void LeaveDepthHack( );
    static Camera * CastHandle( ruNodeHandle handle );
};