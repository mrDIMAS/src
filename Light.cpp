#include "Light.h"
#include "SceneNode.h"

vector< Light* > g_pointLights;
vector< Light* > g_spotLights;

Light * Light::GetLightByHandle( NodeHandle handle ) {
    SceneNode * n = SceneNode::CastHandle( handle );
    Light * l = dynamic_cast< Light* >( n );

    if( l ) {
        return l;
    }

    return 0;
};

Light::Light( int type ) {
    color = Vector3( 1.0f, 1.0f, 1.0f );
    radius = 1.0f;
    SetConeAngles( 45.0f, 80.0f );
    spotTexture = nullptr;

    if( type == LT_POINT ) {
        g_pointLights.push_back( this );
    }

    if( type == LT_SPOT ) {
        g_spotLights.push_back( this );
    }
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
    btVector3 bEye = globalTransform.getOrigin();
    btVector3 bLookAt = bEye + ( globalTransform.getBasis() * btVector3( 0, 1, 0 )).normalize();
    btVector3 bUp = ( globalTransform.getBasis() * btVector3( 0, 0, -1 )).normalize();

    D3DXVECTOR3 dxEye( bEye.x(), bEye.y(), bEye.z() );
    D3DXVECTOR3 dxLookAt( bLookAt.x(), bLookAt.y(), bLookAt.z() );
    D3DXVECTOR3 dxUp( bUp.x(), bUp.y(), bUp.z() );

    D3DXMatrixLookAtRH( &spotProjectionMatrix, &dxEye, &dxLookAt, &dxUp );
}

void Light::SetSpotTexture( Texture * tex ) {
    spotTexture = tex;
}

// API Functions

NodeHandle CreateLight( int type  ) {
    return SceneNode::HandleFromPointer( new Light( type ) );
}

API void SetConeAngles( NodeHandle node, float innerAngle, float outerAngle ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return;
    }

    l->SetConeAngles( innerAngle, outerAngle );
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