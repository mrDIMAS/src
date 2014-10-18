#pragma once

#include "SceneNode.h"
#include "Texture.h"

class Light : public SceneNode {
public:
    float radius;
    Vector3 color;
    float innerAngle;
    float outerAngle;

    float cosHalfInnerAngle;
    float cosHalfOuterAngle;

    D3DXMATRIX spotProjectionMatrix;
    Texture * spotTexture;
public:
    Light( int type );
    virtual ~Light();
    void SetColor( const Vector3 & theColor );
    Vector3 GetColor() const;
    void SetRadius( const float & theRadius );
    float GetRadius() const;
    void SetConeAngles( float theInner, float theOuter );
    float GetInnerAngle() const;
    float GetOuterAngle() const;

    float GetCosHalfInnerAngle( );
    float GetCosHalfOuterAngle( );

    void SetSpotTexture( Texture * tex );
    void BuildSpotProjectionMatrix();

    static Light * GetLightByHandle( NodeHandle handle );
};