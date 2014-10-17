#pragma once

#include "SceneNode.h"

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

    void SetSpotTexture( Texture * tex ) {
        spotTexture = tex;
    }

    void BuildSpotProjectionMatrix() {
        /*
        D3DXVECTOR3 eye = D3DXVECTOR3( globalTransform.getOrigin().x(), globalTransform.getOrigin().y(), globalTransform.getOrigin().z() );
        D3DXVECTOR3 lootAt;

        D3DXMatrixLookAtRH( &spotProjectionMatrix, &eye, &lootAt,   )*/
    }

    static Light * GetLightByHandle( NodeHandle handle );
};