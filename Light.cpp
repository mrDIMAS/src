#include "Light.h"
#include "SceneNode.h"
#include "Utility.h"

vector<Light*> g_pointLights;
vector<Light*> g_spotLights;
vector<Light*> Light::lights;
IDirect3DVertexBuffer9 * Light::flareBuffer = nullptr;
Texture * Light::defaultSpotTexture = nullptr;
CubeTexture * Light::defaultPointCubeTexture = nullptr;

/*
==========
Light::GetLightByHandle
==========
*/
Light * Light::GetLightByHandle( NodeHandle handle ) {
    SceneNode * n = SceneNode::CastHandle( handle );
    Light * light = dynamic_cast< Light* >( n );
    if( !light ) {
        MessageBoxA( 0, Format( "Node '%s' is not a light!", n->name.c_str() ).c_str(), "Error", MB_OK );
        exit( -1 );
    }
    return light;
};
/*
==========
Light::Light
==========
*/
Light::Light( int type ) {
    color = Vector3( 1.0f, 1.0f, 1.0f );
    radius = 1.0f;
    flareTexture = nullptr;    
    pointTexture = nullptr;
    spotTexture = nullptr;
    floating = false;
	brightness = 1.0f;
    if( type == LT_POINT ) {
        g_pointLights.push_back( this );
        if( defaultPointCubeTexture ) {
            pointTexture = defaultPointCubeTexture;
        }
    }
    if( type == LT_SPOT ) {
        g_spotLights.push_back( this );
        if( defaultSpotTexture ) {
            spotTexture = defaultSpotTexture;
        }
    }
    SetConeAngles( 45.0f, 80.0f ); 
}
/*
==========
Light::SetColor
==========
*/
void Light::SetColor( const Vector3 & theColor ) {
    color.x = theColor.x / 255.0f;
    color.y = theColor.y / 255.0f;
    color.z = theColor.z / 255.0f;
}

/*
==========
Light::GetColor
==========
*/
Vector3 Light::GetColor() const {
    return color;
}

/*
==========
Light::SetRadius
==========
*/
void Light::SetRadius( const float & theRadius ) {
    radius = theRadius;
}

/*
==========
Light::GetRadius
==========
*/
float Light::GetRadius() const {
    return radius;
}

/*
==========
Light::GetInnerAngle
==========
*/
float Light::GetInnerAngle() const {
    return innerAngle;
}

/*
==========
Light::GetOuterAngle
==========
*/
float Light::GetOuterAngle() const {
    return outerAngle;
}

/*
==========
Light::SetConeAngles
==========
*/
void Light::SetConeAngles( float theInner, float theOuter ) {
    innerAngle = theInner;
    outerAngle = theOuter;

    cosHalfInnerAngle = cosf( ( innerAngle / 2 ) * SIMD_PI / 180.0f );
    cosHalfOuterAngle = cosf( ( outerAngle / 2 ) * SIMD_PI / 180.0f );
}

/*
==========
Light::GetCosHalfInnerAngle
==========
*/
float Light::GetCosHalfInnerAngle( ) {
    return cosHalfInnerAngle;
}

/*
==========
Light::GetCosHalfOuterAngle
==========
*/
float Light::GetCosHalfOuterAngle( ) {
    return cosHalfOuterAngle;
}

/*
==========
Light::~Light
==========
*/
Light::~Light() {
    auto pointLight = find( g_pointLights.begin(), g_pointLights.end(), this );
    if( pointLight != g_pointLights.end() ) {
        g_pointLights.erase( pointLight );
    }

    auto spotLight = find( g_spotLights.begin(), g_spotLights.end(), this );
    if( spotLight != g_spotLights.end() ) {
        g_spotLights.erase( spotLight );
    }
}

/*
==========
Light::BuildSpotProjectionMatrix
==========
*/
void Light::BuildSpotProjectionMatrixAndFrustum() {
    btVector3 bEye = btVector3( GetRealPosition().x, GetRealPosition().y, GetRealPosition().z );
    btVector3 bLookAt = bEye + globalTransform.getBasis() * btVector3( 0, -1, 0 );
    btVector3 bUp = globalTransform.getBasis() * btVector3( 1, 0, 0 );

    D3DXVECTOR3 dxEye( bEye.x(), bEye.y(), bEye.z() );
    D3DXVECTOR3 dxLookAt( bLookAt.x(), bLookAt.y(), bLookAt.z() );
    D3DXVECTOR3 dxUp( bUp.x(), bUp.y(), bUp.z() );    
   
    D3DXMATRIX mView, mProj;
    D3DXMatrixLookAtRH( &mView, &dxEye, &dxLookAt, &dxUp );
    D3DXMatrixPerspectiveFovRH( &mProj, outerAngle * SIMD_PI / 180.0f, 1.0f, 0.1f, 1000.0f );
    D3DXMatrixMultiply( &spotViewProjectionMatrix, &mView, &mProj );
    frustum.Build( spotViewProjectionMatrix );
}

/*
==========
Light::SetSpotTexture
==========
*/
void Light::SetSpotTexture( Texture * tex ) {
    spotTexture = tex;
}

/*
==========
Light::RenderLightFlares
==========
*/
void Light::RenderLightFlares() {
    if( !flareBuffer ) {
        return;
    }
    IDirect3DStateBlock9 * state;
    g_device->CreateStateBlock( D3DSBT_ALL, &state );
    g_device->SetRenderState( D3DRS_ZWRITEENABLE, false );        
    g_device->SetTransform( D3DTS_VIEW, &g_camera->view );
    g_device->SetTransform( D3DTS_PROJECTION, &g_camera->projection );
    g_device->SetFVF( D3DFVF_XYZ | D3DFVF_TEX1 );
    g_device->SetStreamSource( 0, flareBuffer, 0, sizeof( flareVertex_t ));
    D3DXMATRIX world, scale;
    for( auto light : lights ) {
        if( !light->flareTexture ) {
            continue;
        }            
        btVector3 btOrigin = light->globalTransform.getOrigin();
        float flareScale = (btOrigin - g_camera->globalTransform.getOrigin()).length();
        if( flareScale > 1.2f ) {
            flareScale = 1.2f;
        }                
        D3DXMatrixTranslation( &world, btOrigin.x(), btOrigin.y(), btOrigin.z() );
        D3DXMatrixScaling( &scale, flareScale, flareScale, flareScale );
        D3DXMatrixMultiply( &world, &world, &scale );
        light->flareTexture->Bind( 0 );            
        g_device->SetTransform( D3DTS_WORLD, &world );
        g_device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
    }
    state->Apply();
    state->Release();
}

/*
==========
Light::SetFlare
==========
*/
void Light::SetFlare( Texture * texture ) {
    if( !texture ) {
        return;
    }
    if( !flareBuffer ) {
        flareVertex_t fv[] = 
        {{ -0.5f,  0.5f, 0.0f, 0.0f, 0.0f },
        {  0.5f,  0.5f, 1.0f, 0.0f, 0.0f },
        { -0.5f, -0.5f, 0.0f, 1.0f, 0.0f },
        {  0.5f,  0.5f, 1.0f, 0.0f, 0.0f },
        {  0.5f, -0.5f, 1.0f, 1.0f, 0.0f },
        { -0.5f, -0.5f, 0.0f, 1.0f, 0.0f }};
        g_device->CreateVertexBuffer( sizeof( fv ) / sizeof( fv[0] ), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &flareBuffer, 0 );
    }
    flareTexture = texture;
}

void Light::SetPointTexture( CubeTexture * ctex )
{
    pointTexture = ctex;
}

void Light::DoFloating()
{
    if( floating ) {
        // 'chase' floatTo 
        floatOffset = floatOffset.Lerp( floatTo, 0.015 );
        // if close enough to floatTo
        if( (floatOffset - floatTo).Length2() < 0.005f ) {
            // get new random value
            floatTo.x = frandom( floatMin.x, floatMax.x );
            floatTo.y = frandom( floatMin.y, floatMax.y );
            floatTo.z = frandom( floatMin.z, floatMax.z );
        }
    }
}

Vector3 Light::GetRealPosition()
{
    if( floating ) {
        return GetPosition() + floatOffset;
    } else {
        return GetPosition();
    }
}

// API Functions


/*
==========
GetWorldSpotLightCount
==========
*/
int GetWorldSpotLightCount() {
    return g_spotLights.size();
}

/*
==========
GetWorldSpotLight
==========
*/
NodeHandle GetWorldSpotLight( int n ) {
    NodeHandle handle;
    if( n >= g_spotLights.size() || n < 0 ) {
        return handle;
    } else {
        handle.pointer = g_spotLights[n];
        return handle;
    }
}

/*
==========
GetWorldPointLightCount
==========
*/
int GetWorldPointLightCount() {
    return g_pointLights.size();
}

/*
==========
GetWorldPointLight
==========
*/
NodeHandle GetWorldPointLight( int n ){
    NodeHandle handle;
    if( n >= g_pointLights.size() || n < 0 ) {
        return handle;
    } else {
        handle.pointer = g_pointLights[n];
        return handle;
    }
}

/*
==========
SetLightFlare
==========
*/
void SetLightFlare( NodeHandle node, TextureHandle flareTexture ) {
    Light::GetLightByHandle( node )->flareTexture = (Texture *)flareTexture.pointer;
}

/*
==========
SetLightDefaultFlare
==========
*/
void SetLightDefaultFlare( TextureHandle defaultFlareTexture ) {
    // FIX
}

/*
==========
SetSpotDefaultTexture
==========
*/
void SetSpotDefaultTexture( TextureHandle defaultSpotTexture ) {
    Light::defaultSpotTexture = (Texture *)defaultSpotTexture.pointer;
    for( auto spot : g_spotLights ) {
        if( !spot->spotTexture ) {
            spot->spotTexture = Light::defaultSpotTexture;
        }
    }
}

/*
==========
SetPointDefaultTexture
==========
*/
void SetPointDefaultTexture( CubeTextureHandle defaultPointTexture ) {
    Light::defaultPointCubeTexture = (CubeTexture *)defaultPointTexture.pointer;
    for( auto point : g_pointLights ) {
        if( !point->pointTexture ) {
            point->pointTexture = Light::defaultPointCubeTexture;
        }
    }
}

/*
==========
SetPointTexture
==========
*/
void SetPointTexture( NodeHandle node, CubeTextureHandle cubeTexture ) {
    Light::GetLightByHandle( node )->SetPointTexture( (CubeTexture*)cubeTexture.pointer );
}

/*
==========
CreateLight
==========
*/
NodeHandle CreateLight( int type  ) {
    return SceneNode::HandleFromPointer( new Light( type ) );
}

/*
==========
SetConeAngles
==========
*/
API void SetConeAngles( NodeHandle node, float innerAngle, float outerAngle ) {
    Light::GetLightByHandle( node )->SetConeAngles( innerAngle, outerAngle );
}

/*
==========
SetLightRange
==========
*/
API void SetLightRange( NodeHandle node, float rad ) {
    Light * l = Light::GetLightByHandle( node );
    if( l ) {
        for( int i = 0; i < GetCountChildren( node ); i++ ) {
            SetLightRange( GetChild( node, i ), rad );
        }
        l->SetRadius( rad );
    }
}

/*
==========
SetLightFloatingLimits
==========
*/
API void SetLightFloatingLimits( NodeHandle node, Vector3 floatMin, Vector3 floatMax ) {
    Light * l = Light::GetLightByHandle( node );
    if( l ) {
        l->floatMax = floatMax;
        l->floatMin = floatMin;
    }
}

/*
==========
SetLightFloatingEnabled
==========
*/
API void SetLightFloatingEnabled( NodeHandle node, bool state ) {
    Light::GetLightByHandle( node )->floating = state;
}

/*
==========
IsLightFloatingEnabled
==========
*/
bool IsLightFloatingEnabled( NodeHandle node ) {
    return Light::GetLightByHandle( node )->floating;
}

/*
==========
SetLightColor
==========
*/
API void SetLightColor( NodeHandle node, Vector3 clr ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return;
    }

    for( int i = 0; i < GetCountChildren( node ); i++ ) {
        SetLightColor( GetChild( node, i ), clr );
    }

    l->SetColor( clr );
}

/*
==========
GetLightRange
==========
*/
API float GetLightRange( NodeHandle node ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return 0;
    }

    return l->radius;
}

/*
==========
SetSpotTexture
==========
*/
API void SetSpotTexture( NodeHandle node, TextureHandle texture ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return;
    }

    l->SetSpotTexture( (Texture*)texture.pointer );
}