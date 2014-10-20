#pragma once

#include "SceneNode.h"
#include "Texture.h"
#include "Camera.h"

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

    typedef struct {
        float x,y,z;
        float tx,ty;
    } flareVertex_t;
    static IDirect3DVertexBuffer9 * flareBuffer;
    Texture * flareTexture;
    static vector<Light*> lights;
public:
    explicit Light( int type );
    virtual ~Light();
    void SetColor( const Vector3 & theColor );
    Vector3 GetColor() const;
    void SetRadius( const float & theRadius );
    float GetRadius() const;
    void SetConeAngles( float theInner, float theOuter );
    float GetInnerAngle() const;
    float GetOuterAngle() const;
    void SetFlare( Texture * texture );
    static void RenderLightFlares();
    float GetCosHalfInnerAngle( );
    float GetCosHalfOuterAngle( );
    void SetSpotTexture( Texture * tex );
    void BuildSpotProjectionMatrix();
    static Light * GetLightByHandle( NodeHandle handle );
};