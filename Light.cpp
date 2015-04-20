#include "Precompiled.h"
#include "Engine.h"
#include "Light.h"
#include "SceneNode.h"
#include "Utility.h"

vector< Light* > Light::msPointLightList;
vector< Light* > Light::msSpotLightList;
vector<Light*> Light::lights;
IDirect3DVertexBuffer9 * Light::flareBuffer = nullptr;
Texture * Light::defaultSpotTexture = nullptr;
CubeTexture * Light::defaultPointCubeTexture = nullptr;

/*
==========
Light::GetLightByHandle
==========
*/
Light * Light::GetLightByHandle( ruNodeHandle handle ) {
    SceneNode * n = SceneNode::CastHandle( handle );
    Light * light = dynamic_cast< Light* >( n );
    if( !light ) {
        MessageBoxA( 0, (StringBuilder( "Node " ) << n->mName << " is not a light!").ToCStr(), "Error", MB_OK );
        exit( -1 );
    }
    return light;
};

bool ruIsLightHandeValid( ruNodeHandle handle ) {
	auto pointIter = find( Light::msPointLightList.begin(), Light::msPointLightList.end(), Light::GetLightByHandle( handle ) );
	if( pointIter != Light::msPointLightList.end() ) {
		return true;
	}
	auto spotIter = find( Light::msSpotLightList.begin(), Light::msSpotLightList.end(), Light::GetLightByHandle( handle ) );
	if( spotIter != Light::msSpotLightList.end() ) {
		return true;
	}
	return false;
}

/*
==========
Light::Light
==========
*/
Light::Light( int type ) {
    color = ruVector3( 1.0f, 1.0f, 1.0f );
    radius = 1.0f;
    flareTexture = nullptr;
    pointTexture = nullptr;
    spotTexture = nullptr;
    floating = false;
	mQueryDone = true;
    brightness = 1.0f;
    this->type = type;
    if( type == LT_POINT ) {
        Light::msPointLightList.push_back( this );
        if( defaultPointCubeTexture ) {
            pointTexture = defaultPointCubeTexture;
        }
    }
    if( type == LT_SPOT ) {
        Light::msSpotLightList.push_back( this );
        if( defaultSpotTexture ) {
            spotTexture = defaultSpotTexture;
        }
    }
    SetConeAngles( 45.0f, 80.0f );
	Engine::Instance().GetDevice()->CreateQuery( D3DQUERYTYPE_OCCLUSION, &pQuery );
	trulyVisible = true;
	inFrustum = false;
}
/*
==========
Light::SetColor
==========
*/
void Light::SetColor( const ruVector3 & theColor ) {
    color.x = theColor.x / 255.0f;
    color.y = theColor.y / 255.0f;
    color.z = theColor.z / 255.0f;
}

/*
==========
Light::GetColor
==========
*/
ruVector3 Light::GetColor() const {
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
    auto pointLight = find( Light::msPointLightList.begin(), Light::msPointLightList.end(), this );
    if( pointLight != Light::msPointLightList.end() ) {
        Light::msPointLightList.erase( pointLight );
    }

    auto spotLight = find( Light::msSpotLightList.begin(), Light::msSpotLightList.end(), this );
    if( spotLight != Light::msSpotLightList.end() ) {
        Light::msSpotLightList.erase( spotLight );
    }

	pQuery->Release();
}

/*
==========
Light::BuildSpotProjectionMatrix
==========
*/
void Light::BuildSpotProjectionMatrixAndFrustum() {
    btVector3 bEye = btVector3( GetRealPosition().x, GetRealPosition().y, GetRealPosition().z );
    btVector3 bLookAt = bEye + mGlobalTransform.getBasis() * btVector3( 0, -1, 0 );
    btVector3 bUp = mGlobalTransform.getBasis() * btVector3( 1, 0, 0 );

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
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, false ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTransform( D3DTS_VIEW, &Camera::msCurrentCamera->mView ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTransform( D3DTS_PROJECTION, &Camera::msCurrentCamera->mProjection ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_TEX1 ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetStreamSource( 0, flareBuffer, 0, sizeof( flareVertex_t )));
    D3DXMATRIX world, scale;
    for( auto light : lights ) {
        if( !light->flareTexture ) {
            continue;
        }
        btVector3 btOrigin = light->mGlobalTransform.getOrigin();
        float flareScale = (btOrigin - Camera::msCurrentCamera->mGlobalTransform.getOrigin()).length();
        if( flareScale > 1.2f ) {
            flareScale = 1.2f;
        }
        D3DXMatrixTranslation( &world, btOrigin.x(), btOrigin.y(), btOrigin.z() );
        D3DXMatrixScaling( &scale, flareScale, flareScale, flareScale );
        D3DXMatrixMultiply( &world, &world, &scale );
        light->flareTexture->Bind( 0 );
        CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTransform( D3DTS_WORLD, &world ));
        CheckDXErrorFatal( Engine::Instance().GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 ));
    }
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
        flareVertex_t fv[] = {
            { -0.5f,  0.5f, 0.0f, 0.0f, 0.0f },
            {  0.5f,  0.5f, 1.0f, 0.0f, 0.0f },
            { -0.5f, -0.5f, 0.0f, 1.0f, 0.0f },
            {  0.5f,  0.5f, 1.0f, 0.0f, 0.0f },
            {  0.5f, -0.5f, 1.0f, 1.0f, 0.0f },
            { -0.5f, -0.5f, 0.0f, 1.0f, 0.0f }
        };
        CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateVertexBuffer( sizeof( fv ) / sizeof( fv[0] ), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &flareBuffer, 0 ));
    }
    flareTexture = texture;
}

void Light::SetPointTexture( CubeTexture * ctex ) {
    pointTexture = ctex;
}

void Light::DoFloating() {
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

ruVector3 Light::GetRealPosition() {
    if( floating ) {
        return GetPosition() + floatOffset;
    } else {
        return GetPosition();
    }
}

// API Functions
int ruGetWorldSpotLightCount() {
    return Light::msSpotLightList.size();
}

ruNodeHandle ruGetWorldSpotLight( int n ) {
    ruNodeHandle handle;
    if( n >= Light::msSpotLightList.size() || n < 0 ) {
        return handle;
    } else {
        handle.pointer = Light::msSpotLightList[n];
        return handle;
    }
}

int ruGetWorldPointLightCount() {
    return Light::msPointLightList.size();
}

ruNodeHandle ruGetWorldPointLight( int n ) {
    ruNodeHandle handle;
    if( n >= Light::msPointLightList.size() || n < 0 ) {
        return handle;
    } else {
        handle.pointer = Light::msPointLightList[n];
        return handle;
    }
}

void ruSetLightFlare( ruNodeHandle node, ruTextureHandle flareTexture ) {
    Light::GetLightByHandle( node )->flareTexture = (Texture *)flareTexture.pointer;
}

void ruSetLightDefaultFlare( ruTextureHandle defaultFlareTexture ) {
    // FIX
}

bool ruIsLight( ruNodeHandle node ) {
    Light * pLight = dynamic_cast< Light* >( (SceneNode*)node.pointer );
    return pLight != nullptr;
}

void ruSetLightSpotDefaultTexture( ruTextureHandle defaultSpotTexture ) {
    Light::defaultSpotTexture = (Texture *)defaultSpotTexture.pointer;
    for( auto spot : Light::msSpotLightList ) {
        if( !spot->spotTexture ) {
            spot->spotTexture = Light::defaultSpotTexture;
        }
    }
}

void ruSetLightPointDefaultTexture( ruCubeTextureHandle defaultPointTexture ) {
    Light::defaultPointCubeTexture = (CubeTexture *)defaultPointTexture.pointer;
    for( auto point : Light::msPointLightList ) {
        if( !point->pointTexture ) {
            point->pointTexture = Light::defaultPointCubeTexture;
        }
    }
}

void ruSetLightPointTexture( ruNodeHandle node, ruCubeTextureHandle cubeTexture ) {
    Light::GetLightByHandle( node )->SetPointTexture( (CubeTexture*)cubeTexture.pointer );
}

ruNodeHandle ruCreateLight( int type  ) {
    return SceneNode::HandleFromPointer( new Light( type ) );
}

void ruSetConeAngles( ruNodeHandle node, float innerAngle, float outerAngle ) {
    Light::GetLightByHandle( node )->SetConeAngles( innerAngle, outerAngle );
}

void ruSetLightRange( ruNodeHandle node, float rad ) {
    Light * l = Light::GetLightByHandle( node );
    if( l ) {
        for( int i = 0; i < ruGetNodeCountChildren( node ); i++ ) {
            ruSetLightRange( ruGetNodeChild( node, i ), rad );
        }
        l->SetRadius( rad );
    }
}

void ruSetLightFloatingLimits( ruNodeHandle node, ruVector3 floatMin, ruVector3 floatMax ) {
    Light * l = Light::GetLightByHandle( node );
    if( l ) {
        l->floatMax = floatMax;
        l->floatMin = floatMin;
    }
}

void ruSetLightFloatingEnabled( ruNodeHandle node, bool state ) {
    Light::GetLightByHandle( node )->floating = state;
}

bool ruIsLightFloatingEnabled( ruNodeHandle node ) {
    return Light::GetLightByHandle( node )->floating;
}

void ruSetLightColor( ruNodeHandle node, ruVector3 clr ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return;
    }

    for( int i = 0; i < ruGetNodeCountChildren( node ); i++ ) {
        ruSetLightColor( ruGetNodeChild( node, i ), clr );
    }

    l->SetColor( clr );
}

float ruGetLightRange( ruNodeHandle node ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return 0;
    }

    return l->radius;
}

void ruSetLightSpotTexture( ruNodeHandle node, ruTextureHandle texture ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return;
    }

    l->SetSpotTexture( (Texture*)texture.pointer );
}

bool ruIsLightSeePoint( ruNodeHandle node, ruVector3 point ) {
    Light * l = Light::GetLightByHandle( node );

    if( !l ) {
        return false;
    }

    if( l->type == LT_SPOT ) {
        bool inFrustum = l->frustum.IsPointInside( point );

        if( inFrustum ) {
            return ( ruVector3( l->mGlobalTransform.getOrigin().m_floats ) - point ).Length2() < l->radius * l->radius * 4;
        }
    } else {
        return (ruVector3( l->mGlobalTransform.getOrigin().m_floats ) - point ).Length2() < l->radius * l->radius * 4;
    }
    return false;
}