#include "DeferredRenderer.h"
#include "Light.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Camera.h"
#include "Skybox.h"
#include "Utility.h"
#include "Renderer.h"

DeferredRenderer * g_deferredRenderer = 0;

bool g_fxaaEnabled = true;

bool IsTextureFormatOk( D3DFORMAT TextureFormat ) {
    return SUCCEEDED( g_d3d->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_TEXTURE, TextureFormat) );
}

DeferredRenderer::DeferredRenderer() {
    effectsQuad = new EffectsQuad;
    debugQuad = new EffectsQuad( true );
    CreateBoundingVolumes();
    SetRenderingQuality( 1 );
    pFXAA = new FXAA;
    gBuffer = new GBuffer;
    pass2SpotLight = new Pass2SpotLight;
    pass2AmbientLight = new Pass2AmbientLight;
    pass2PointLight = new Pass2PointLight;
    bvRenderer = new BoundingVolumeRenderingShader;
    spotShadowMap = new SpotlightShadowMap;
    pointShadowMap = new PointlightShadowMap;
    // check support of floating-point textures first
    if( IsTextureFormatOk( D3DFMT_A16B16G16R16 )) {
        pHDRRenderer = new HDRRenderer( D3DFMT_A16B16G16R16 );
    } else {
        pHDRRenderer = nullptr;
    }
}

DeferredRenderer::~DeferredRenderer() {
    icosphere->Release();
    cone->Release();
    delete gBuffer;
    delete effectsQuad;
    delete pass2SpotLight;
    delete pass2AmbientLight;
    delete pass2PointLight;
    delete bvRenderer;
    delete spotShadowMap;
    delete pFXAA;
    delete pointShadowMap;
    delete pHDRRenderer;
    delete debugQuad;
}

GBuffer * DeferredRenderer::GetGBuffer() {
    return gBuffer;
}

struct XYZNormalVertex {
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
};

void DeferredRenderer::CreateBoundingVolumes() {
    int quality = 6;

    D3DXCreateSphere( gpDevice, 1.0, quality, quality, &icosphere, 0 );

    D3DXCreateCylinder( gpDevice, 0.0f, 1.0f, 1.0f, quality, quality, &cone, 0 );

    // rotate cylinder on 90 degrees
    XYZNormalVertex * data;
    cone->LockVertexBuffer( 0, (void**)&data );
    D3DXMATRIX tran;
    D3DXMatrixTranslation( &tran, 0, -0.5, 0 );
    D3DXMATRIX rot90;
    D3DXMatrixRotationAxis( &rot90, &D3DXVECTOR3( 1, 0, 0 ), SIMD_HALF_PI );
    D3DXMATRIX transform;
    D3DXMatrixMultiply( &transform, &rot90, &tran );

    for( int i = 0; i < cone->GetNumVertices(); i++ ) {
        XYZNormalVertex * v = &data[ i ];

        D3DXVec3TransformCoord( &v->p, &v->p, &transform );
    }

    cone->UnlockVertexBuffer();
}

////////////////////////////////////////////////////////////
// Point Light Subclass
////////////////////////////////////////////////////////////
DeferredRenderer::Pass2PointLight::Pass2PointLight() {
    pixelShader = new PixelShader( "data/shaders/deferredPointLightHQ.pso", true );
    pixelShaderLQ = new PixelShader( "data/shaders/deferredPointLightLQ.pso", true );
}

void DeferredRenderer::Pass2PointLight::Bind( D3DXMATRIX & invViewProj ) {
    BindShader();
    gpRenderer->SetPixelShaderFloat3( 8, g_camera->globalTransform.getOrigin().m_floats );
    gpRenderer->SetPixelShaderMatrix( 0, &invViewProj );
}

void DeferredRenderer::Pass2PointLight::BindShader( ) {
    if( g_deferredRenderer->renderQuality > 0 ) {
        pixelShader->Bind();
    } else {
        pixelShaderLQ->Bind();
    }
}

void DeferredRenderer::Pass2PointLight::SetLight( Light * light ) {
	// position
    gpRenderer->SetPixelShaderFloat3( 5, light->GetRealPosition().elements ); 
	// color
    gpRenderer->SetPixelShaderFloat3( 6, light->GetColor().elements ); 
	 // range
    gpRenderer->SetPixelShaderFloat( 7, powf( light->GetRadius(), 4 ));
	// brightness
    gpRenderer->SetPixelShaderFloat( 9, g_hdrEnabled ? light->brightness : 1.0f ); 
	// bounding volume render
	gpRenderer->SetPixelShaderBool( 1, FALSE );
    if( light->pointTexture ) {
        gpDevice->SetTexture( 3, light->pointTexture->cubeTexture );
        gpRenderer->SetPixelShaderBool( 0, 1 );
    } else {
        gpDevice->SetTexture( 3, 0 );
        gpRenderer->SetPixelShaderBool( 0, 0 );
    }
}

DeferredRenderer::Pass2PointLight::~Pass2PointLight() {
    delete pixelShader;
}

////////////////////////////////////////////////////////////
// Ambient Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::Pass2AmbientLight::Pass2AmbientLight() {
    pixelShader = new PixelShader( "data/shaders/deferredAmbientLight.pso", true );
}

void DeferredRenderer::Pass2AmbientLight::Bind( ) {
    pixelShader->Bind();
    gpRenderer->SetPixelShaderFloat3( 0, g_ambientColor.elements );
}

DeferredRenderer::Pass2AmbientLight::~Pass2AmbientLight() {
    delete pixelShader;
}

////////////////////////////////////////////////////////////
// Spot Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::Pass2SpotLight::Pass2SpotLight( ) {
    pixelShader = new PixelShader( "data/shaders/deferredSpotLight.pso", true );
}

void DeferredRenderer::Pass2SpotLight::BindShader( ) {
    pixelShader->Bind();
}

void DeferredRenderer::Pass2SpotLight::Bind( D3DXMATRIX & invViewProj ) {
    BindShader();

    gpRenderer->SetPixelShaderFloat3( 13, g_camera->globalTransform.getOrigin().m_floats );
    gpRenderer->SetPixelShaderMatrix( 0, &invViewProj );
}

void DeferredRenderer::Pass2SpotLight::SetLight( Light * lit ) {
    btVector3 direction = ( lit->globalTransform.getBasis() * btVector3( 0, 1, 0 )).normalize();
    // position
    gpRenderer->SetPixelShaderFloat3( 10, lit->GetRealPosition().elements );
    // range
    gpRenderer->SetPixelShaderFloat( 14, powf( lit->GetRadius(), 4 ));
    // color
    gpRenderer->SetPixelShaderFloat3( 11, lit->GetColor().elements );
    // inner angle
    gpRenderer->SetPixelShaderFloat( 15, lit->GetCosHalfInnerAngle() );
    // outer angle
    gpRenderer->SetPixelShaderFloat( 16, lit->GetCosHalfOuterAngle() );
    // direction
    gpRenderer->SetPixelShaderFloat3( 12, direction.m_floats );
    // use shadows
    gpRenderer->SetPixelShaderBool( 1, g_useSpotLightShadows ? TRUE : FALSE );
    // brightness
    gpRenderer->SetPixelShaderFloat( 17, (g_hdrEnabled ? lit->brightness : 1.0f) );
	// bounding volume render
	gpRenderer->SetPixelShaderBool( 2, FALSE );
    if( lit->spotTexture || g_useSpotLightShadows ) {
        lit->BuildSpotProjectionMatrixAndFrustum();
        if( lit->spotTexture ) {
            lit->spotTexture->Bind( 3 );
            // use spot texture
            gpRenderer->SetPixelShaderBool( 0, TRUE );
        }
        // spot view matrix
        gpRenderer->SetPixelShaderMatrix( 5, &lit->spotViewProjectionMatrix );
    } else {
        gpDevice->SetTexture( 3, nullptr );
        // use spot texture
        if( !lit->spotTexture ) {
            gpRenderer->SetPixelShaderBool( 0, FALSE );
        }
    }
}

DeferredRenderer::Pass2SpotLight::~Pass2SpotLight() {
    delete pixelShader;
}

//////////////////////////////////////////////////////////////////////////
// Bounding volume rendering shader
//
// Bounding volume for a light can be a sphere for point light
// a oriented cone for a spot light
DeferredRenderer::BoundingVolumeRenderingShader::BoundingVolumeRenderingShader() {
    vs = new VertexShader( "data/shaders/boundingVolume.vso", true );
    ps = new PixelShader( "data/shaders/boundingVolume.pso", true );
    D3DVERTEXELEMENT9 vd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        D3DDECL_END()
    };
    gpDevice->CreateVertexDeclaration( vd, &vertexDeclaration ) ;
}

DeferredRenderer::BoundingVolumeRenderingShader::~BoundingVolumeRenderingShader() {
    delete ps;
    delete vs;
    vertexDeclaration->Release();
}

void DeferredRenderer::BoundingVolumeRenderingShader::Bind() {
    ps->Bind();
    vs->Bind();
    gpDevice->SetVertexDeclaration( vertexDeclaration );
}

void DeferredRenderer::BoundingVolumeRenderingShader::SetTransform( D3DXMATRIX & wvp ) {
    gpRenderer->SetVertexShaderMatrix( 0, &wvp );
}

void DeferredRenderer::RenderIcosphereIntoStencilBuffer( Light * pLight ) {
    ruVector3 realPosition = pLight->GetRealPosition();
    float scl = 2.5f * pLight->radius;
    D3DXMATRIX world;
    world._11 = scl;
    world._12 = 0.0f;
    world._13 = 0.0f;
    world._14 = 0.0f;
    world._21 = 0.0f;
    world._22 = scl;
    world._23 = 0.0f;
    world._24 = 0.0f;
    world._31 = 0.0f;
    world._32 = 0.0f;
    world._33 = scl;
    world._34 = 0.0f;
    world._41 = realPosition.x;
    world._42 = realPosition.y;
    world._43 = realPosition.z;
    world._44 = 1.0f;
    bvRenderer->Bind();
    D3DXMATRIX wvp;
    D3DXMatrixMultiply( &wvp, &world, &g_camera->viewProjection );
    bvRenderer->SetTransform( wvp );
    icosphere->DrawSubset( 0 );
}

void DeferredRenderer::RenderConeIntoStencilBuffer( Light * lit ) {
    float height = lit->GetRadius() * 2.05;
    float radius = height * sinf( ( lit->GetOuterAngle() * 0.75f ) * SIMD_PI / 180.0f );
    D3DXMATRIX scale;
    D3DXMatrixScaling( &scale, radius, height, radius );
    D3DXMATRIX world;
    GetD3DMatrixFromBulletTransform( lit->globalTransform, world );
    D3DXMatrixMultiply( &world, &scale, &world );
    bvRenderer->Bind();
    D3DXMATRIX wvp;
    D3DXMatrixMultiply( &wvp, &world, &g_camera->viewProjection );
    bvRenderer->SetTransform( wvp );
    cone->DrawSubset( 0 );
}

void DeferredRenderer::EndFirstPassAndDoSecondPass() {
    OnEnd();

    if( pHDRRenderer && g_hdrEnabled ) {
        pHDRRenderer->SetAsRenderTarget();
    } else {
        if( g_fxaaEnabled ) {
            pFXAA->BeginDrawIntoTexture();
        } else {
            gBuffer->BindBackSurfaceAsRT();
        }
    }

    gpDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
	
    if( g_camera->skybox ) {
        g_camera->skybox->Render( g_camera->globalTransform.getOrigin() );
    }  

    gBuffer->BindTextures();
	gpDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    pass2AmbientLight->Bind();
    effectsQuad->Bind();
    effectsQuad->Render();
	gpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	gpDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

    // Render point lights
    for( auto pLight : g_pointLightList ) {
        if( g_usePointLightShadows ) {
            IDirect3DSurface9 * prevSurface = nullptr;
            if( pHDRRenderer && g_hdrEnabled ) {
                prevSurface = pHDRRenderer->hdrSurface;
            } else if( g_fxaaEnabled ) {
                prevSurface = pFXAA->renderTarget;
            } else {
                prevSurface = gBuffer->backSurface;
            }
            pointShadowMap->UnbindShadowCubemap( 4 );
            pointShadowMap->RenderPointShadowMap( prevSurface, 0, pLight );
            pointShadowMap->BindShadowCubemap( 4 );
        }


        gpDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
        gpDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
        gpDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
		// bounding volume rendering, lighting shader var
		gpRenderer->SetPixelShaderBool( 1, TRUE );
        RenderIcosphereIntoStencilBuffer( pLight );

        pass2PointLight->Bind( g_camera->invViewProjection );
        pass2PointLight->SetLight( pLight );

        effectsQuad->Bind();		

        gpDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
        gpDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
        gpDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );
        // quad render
        effectsQuad->Render();
    }

    // Render spot lights
    for( auto pLight : g_spotLightList ) {
        if( g_useSpotLightShadows ) {
            IDirect3DSurface9 * prevSurface = nullptr;
            if( pHDRRenderer && g_hdrEnabled ) {
                prevSurface = pHDRRenderer->hdrSurface;
            } else if( g_fxaaEnabled ) {
                prevSurface = pFXAA->renderTarget;
            } else {
                prevSurface = gBuffer->backSurface;
            }
            spotShadowMap->UnbindSpotShadowMap( 4 );

            gpDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
            gpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
            gpDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

            spotShadowMap->RenderSpotShadowMap( prevSurface, 0, pLight );

            gpDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
            gpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
            gpDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

            spotShadowMap->BindSpotShadowMap( 4 );
        }

        gpDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
        gpDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
        gpDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

        RenderConeIntoStencilBuffer( pLight );

        pass2SpotLight->Bind( g_camera->invViewProjection );
        pass2SpotLight->SetLight( pLight );

        gpDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
        gpDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
        gpDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );

        effectsQuad->Bind();
        effectsQuad->Render();
    }

    if( pHDRRenderer && g_hdrEnabled ) {
        pHDRRenderer->CalculateFrameLuminance( );
        if( g_fxaaEnabled ) {
            pHDRRenderer->DoToneMapping( pFXAA->renderTarget );
            pFXAA->DoAntialiasing( pFXAA->texture );
        } else {
            pHDRRenderer->DoToneMapping( gBuffer->backSurface );
        }
    } else {
        if( g_fxaaEnabled ) {
            CheckDXErrorFatal( gpDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE ));
            CheckDXErrorFatal( gpDevice->SetRenderState( D3DRS_ZENABLE, FALSE ));
            pFXAA->DoAntialiasing( pFXAA->texture );
        }
    }
}

void DeferredRenderer::SetPointLightShadowMapSize( int size ) {
    if( size != pointShadowMap->iSize ) {
        if( pointShadowMap ) {
            delete pointShadowMap;
        }
        pointShadowMap = new PointlightShadowMap( size );
    }
}

void DeferredRenderer::SetSpotLightShadowMapSize( int size ) {
    if( size != spotShadowMap->iSize ) {
        if( spotShadowMap ) {
            delete spotShadowMap;
        }
        spotShadowMap = new SpotlightShadowMap( size );
    }
}

void DeferredRenderer::SetRenderingQuality( char quality ) {
    renderQuality = quality;
    if( renderQuality < 0 ) {
        renderQuality = 0;
    }
    if( renderQuality > 1 ) {
        renderQuality = 1;
    }
}
