#include "Light.h"
#include "SceneNode.h"

vector< Light* > g_pointLights;
vector< Light* > g_spotLights;

Light * Light::GetLightByHandle( NodeHandle handle ) {
    SceneNode * n = SceneNode::CastHandle( handle );
    Light * l = dynamic_cast< Light* >( n );

    if( l )
        return l;

    return 0;
};

Light::Light( int type ) {
    color = Vector3( 1.0f, 1.0f, 1.0f );
    radius = 1.0f;
    innerAngle = 90.0f;
    outerAngle = 90.0f;

    if( type == LT_POINT )
        g_pointLights.push_back( this );

    if( type == LT_SPOT )
        g_spotLights.push_back( this );
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

    cosHalfInnerAngle = cosf( ( innerAngle * 0.5 ) * SIMD_PI / 180.0f );
    cosHalfOuterAngle = cosf( ( outerAngle * 0.5 ) * SIMD_PI / 180.0f );
}

float Light::GetCosHalfInnerAngle( ) {
    return cosHalfInnerAngle;
}

float Light::GetCosHalfOuterAngle( ) {
    return cosHalfOuterAngle;
}

Light::~Light() {
    auto pointLight = find( g_pointLights.begin(), g_pointLights.end(), this );
    if( pointLight != g_pointLights.end() )
        g_pointLights.erase( pointLight );

    auto spotLight = find( g_spotLights.begin(), g_spotLights.end(), this );
    if( spotLight != g_spotLights.end() )
        g_spotLights.erase( spotLight );
}

// API Functions

NodeHandle CreateLight( int type  ) {
    return SceneNode::HandleFromPointer( new Light( type ) );
}

API void SetConeAngles( NodeHandle node, float innerAngle, float outerAngle ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l )
        return;

    l->SetConeAngles( innerAngle, outerAngle );
}

API void SetLightRange( NodeHandle node, float rad ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l )
        return;

    for( int i = 0; i < GetCountChildren( node ); i++ ) {
        SetLightRange( GetChild( node, i ), rad );
    }

    l->SetRadius( rad );
}

API void SetLightColor( NodeHandle node, Vector3 clr ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l )
        return;

    for( int i = 0; i < GetCountChildren( node ); i++ ) {
        SetLightColor( GetChild( node, i ), clr );
    }

    l->SetColor( clr );
}

API float GetLightRange( NodeHandle node ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l )
        return 0;

    return l->radius;
}