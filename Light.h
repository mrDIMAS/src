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
	IDirect3DQuery9 * pQuery;
	bool mQueryDone;
	bool trulyVisible;
	bool inFrustum;
    typedef struct {
        float x,y,z;
        float tx,ty;
    } flareVertex_t;
    Frustum frustum;
    float radius;
    ruVector3 color;
    float innerAngle;
    float outerAngle;
    float cosHalfInnerAngle;
    float cosHalfOuterAngle;
    bool floating;
    float brightness;
    int type;
    ruVector3 floatMin;
    ruVector3 floatMax;
    ruVector3 floatTo;
    ruVector3 floatOffset;
    D3DXMATRIX spotViewProjectionMatrix;
    Texture * spotTexture;
    CubeTexture * pointTexture;
    static IDirect3DVertexBuffer9 * flareBuffer;
    Texture * flareTexture;

    static vector<Light*> lights;
    static Texture * defaultSpotTexture;
    static CubeTexture * defaultPointCubeTexture;
	static vector< Light* > msPointLightList;
	static vector< Light* > msSpotLightList; 
public:
    explicit Light( int type );
    virtual ~Light();
    void SetColor( const ruVector3 & theColor );
    ruVector3 GetColor() const;
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
    static Light * GetLightByHandle( ruNodeHandle handle );
    ruVector3 GetRealPosition( );
    void DoFloating();
};