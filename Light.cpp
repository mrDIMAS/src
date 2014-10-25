#include "Light.h"
#include "SceneNode.h"
#include "Utility.h"

vector<Light*> g_pointLights;
vector<Light*> g_spotLights;
vector<Light*> Light::lights;
IDirect3DVertexBuffer9 * Light::flareBuffer = nullptr;
Texture * Light::defaultSpotTexture = nullptr;
CubeTexture * Light::defaultPointCubeTexture = nullptr;

Light * Light::GetLightByHandle( NodeHandle handle ) {
    SceneNode * n = SceneNode::CastHandle( handle );
    Light * light = dynamic_cast< Light* >( n );
    if( !light ) {
        MessageBoxA( 0, Format( "Node '%s' is not a light!", n->name.c_str() ).c_str(), "Error", MB_OK );
        exit( -1 );
    }
    return light;
};

Light::Light( int type ) {
    color = Vector3( 1.0f, 1.0f, 1.0f );
    radius = 1.0f;
    flareTexture = nullptr;    
    pointTexture = nullptr;
    spotTexture = nullptr;
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

void Light::SetColor( const Vector3 & theColor ) {
    color.x = theColor.x / 255.0f;
    color.y = theColor.y / 255.0f;
    color.z = theColor.z / 255.0f;
}

Vector3 Light::GetColor() const {
    return color;
}

void Light::SetRadius( const float & theRadius ) {
    radius = theRadius;
}

float Light::GetRadius() const {
    return radius;
}


float Light::GetInnerAngle() const {
    return innerAngle;
}

float Light::GetOuterAngle() const {
    return outerAngle;
}

void Light::SetConeAngles( float theInner, float theOuter ) {
    innerAngle = theInner;
    outerAngle = theOuter;

    cosHalfInnerAngle = cosf( ( innerAngle / 2 ) * SIMD_PI / 180.0f );
    cosHalfOuterAngle = cosf( ( outerAngle / 2 ) * SIMD_PI / 180.0f );
}

float Light::GetCosHalfInnerAngle( ) {
    return cosHalfInnerAngle;
}

float Light::GetCosHalfOuterAngle( ) {
    return cosHalfOuterAngle;
}

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

void Light::BuildSpotProjectionMatrix() {
    //CalculateGlobalTransform();
    
    /*
    btVector3 bEye = globalTransform.getOrigin();
    btVector3 bLookAt = bEye + globalTransform.getBasis() * btVector3( 0, 1, 0 );
    btVector3 bUp = globalTransform.getBasis() * btVector3( 0, 0, -1 );
    
    D3DXVECTOR3 dxEye( bEye.x(), bEye.y(), bEye.z() );
    D3DXVECTOR3 dxLookAt( bLookAt.x(), bLookAt.y(), bLookAt.z() );
    D3DXVECTOR3 dxUp( bUp.x(), bUp.y(), bUp.z() );
    */
    
    D3DXVECTOR3 dxEye( 40, 40, 40 );
    D3DXVECTOR3 dxLookAt( 0, 0, 0 );
    D3DXVECTOR3 dxUp( 0, 1, 0 );
    
    D3DXMATRIX mView, mProj;
    D3DXMatrixLookAtRH( &mView, &dxEye, &dxLookAt, &dxUp );
    D3DXMatrixPerspectiveFovRH( &mProj, outerAngle * SIMD_PI / 180.0f, (float)g_width / (float)g_height, g_camera->nearZ, g_camera->farZ );
    D3DXMatrixMultiply( &spotViewProjectionMatrix, &mView, &mProj );
}

D3DXMATRIX Light::BuiltSpotProjectionViewMatrix()
{
    D3DXVECTOR3 dxEye( 40, 40, 40 );
    D3DXVECTOR3 dxLookAt( 0, 0, 0 );
    D3DXVECTOR3 dxUp( 0, 1, 0 );

    D3DXMATRIX mView, mProj, out;
    D3DXMatrixLookAtRH( &mView, &dxEye, &dxLookAt, &dxUp );
    D3DXMatrixPerspectiveFovRH( &mProj, outerAngle * SIMD_PI / 180.0f, (float)g_width / (float)g_height, g_camera->nearZ, g_camera->farZ );
    D3DXMatrixMultiply( &out, &mProj, &mView );

    return out;
}

void Light::SetSpotTexture( Texture * tex ) {
    spotTexture = tex;
}

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

// API Functions



int GetWorldSpotLightCount() {
    return g_spotLights.size();
}

NodeHandle GetWorldSpotLight( int n ) {
    NodeHandle handle;
    if( n >= g_spotLights.size() || n < 0 ) {
        return handle;
    } else {
        handle.pointer = g_spotLights[n];
        return handle;
    }
}

int GetWorldPointLightCount() {
    return g_pointLights.size();
}

NodeHandle GetWorldPointLight( int n ){
    NodeHandle handle;
    if( n >= g_pointLights.size() || n < 0 ) {
        return handle;
    } else {
        handle.pointer = g_pointLights[n];
        return handle;
    }
}

void SetLightFlare( NodeHandle node, TextureHandle flareTexture ) {
    Light::GetLightByHandle( node )->flareTexture = (Texture *)flareTexture.pointer;
}

void SetLightDefaultFlare( TextureHandle defaultFlareTexture ) {

}

void SetSpotDefaultTexture( TextureHandle defaultSpotTexture ) {
    Light::defaultSpotTexture = (Texture *)defaultSpotTexture.pointer;
    for( auto spot : g_spotLights ) {
        if( !spot->spotTexture ) {
            spot->spotTexture = Light::defaultSpotTexture;
        }
    }
}

void SetPointDefaultTexture( CubeTextureHandle defaultPointTexture ) {
    Light::defaultPointCubeTexture = (CubeTexture *)defaultPointTexture.pointer;
    for( auto point : g_pointLights ) {
        if( !point->pointTexture ) {
            point->pointTexture = Light::defaultPointCubeTexture;
        }
    }
}

void SetPointTexture( NodeHandle node, CubeTextureHandle cubeTexture ) {
    Light::GetLightByHandle( node )->SetPointTexture( (CubeTexture*)cubeTexture.pointer );
}

NodeHandle CreateLight( int type  ) {
    return SceneNode::HandleFromPointer( new Light( type ) );
}

API void SetConeAngles( NodeHandle node, float innerAngle, float outerAngle ) {
    Light::GetLightByHandle( node )->SetConeAngles( innerAngle, outerAngle );
}

API void SetLightRange( NodeHandle node, float rad ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return;
    }

    for( int i = 0; i < GetCountChildren( node ); i++ ) {
        SetLightRange( GetChild( node, i ), rad );
    }

    l->SetRadius( rad );
}

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

API float GetLightRange( NodeHandle node ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return 0;
    }

    return l->radius;
}

API void SetSpotTexture( NodeHandle node, TextureHandle texture ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return;
    }

    l->SetSpotTexture( (Texture*)texture.pointer );
}