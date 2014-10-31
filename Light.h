#pragma once

#include "SceneNode.h"
#include "Texture.h"
#include "Camera.h"
#include "CubeTexture.h"
#include "Mesh.h"
#include "AABB.h"
#include "Frustum.h"
#include "Utility.h"

class Light : public SceneNode {
public:
    typedef struct {
        float x,y,z;
        float tx,ty;
    } flareVertex_t;
    Frustum frustum;
    float radius;
    Vector3 color;
    float innerAngle;
    float outerAngle;
    float cosHalfInnerAngle;
    float cosHalfOuterAngle;
    bool floating;
    Vector3 floatMin;
    Vector3 floatMax;
    Vector3 floatTo;
    Vector3 floatOffset;
    D3DXMATRIX spotViewProjectionMatrix;
    Texture * spotTexture;
    CubeTexture * pointTexture;
    static IDirect3DVertexBuffer9 * flareBuffer;
    Texture * flareTexture;
    static vector<Light*> lights;
    static Texture * defaultSpotTexture;
    static CubeTexture * defaultPointCubeTexture;
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
    void SetPointTexture( CubeTexture * ctex );
    void BuildSpotProjectionMatrixAndFrustum();
    static Light * GetLightByHandle( NodeHandle handle );
    Vector3 GetRealPosition( );
    void DoFloating();
};