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
    mFullscreenQuad = new EffectsQuad;
    mDebugQuad = new EffectsQuad( true );
    CreateBoundingVolumes();
    mFXAA = new FXAA;
    mGBuffer = new GBuffer;
    mSpotLightShader = new SpotLightShader;
    mAmbientLightShader = new AmbientLightShader;
    mPointLightShader = new PointLightShader;
    bvRenderer = new BoundingVolumeRenderingShader;
    mSpotLightShadowMap = new SpotlightShadowMap;
    // check support of floating-point textures first
    if( IsTextureFormatOk( D3DFMT_A16B16G16R16 )) {
        mHDRShader = new HDRShader( D3DFMT_A16B16G16R16 );
    } else {
        mHDRShader = nullptr;
    }
}

DeferredRenderer::~DeferredRenderer() {
    icosphere->Release();
    cone->Release();
    delete mGBuffer;
    delete mFullscreenQuad;
    delete mSpotLightShader;
    delete mAmbientLightShader;
    delete mPointLightShader;
    delete bvRenderer;
    delete mSpotLightShadowMap;
    delete mFXAA;
    delete mHDRShader;
    delete mDebugQuad;
}

GBuffer * DeferredRenderer::GetGBuffer() {
    return mGBuffer;
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
DeferredRenderer::PointLightShader::PointLightShader() {
    pixelShader = new PixelShader( "data/shaders/deferredPointLight.pso", true );
    pixelShaderTexProj = new PixelShader( "data/shaders/deferredPointLightTexProj.pso", true );
}

void DeferredRenderer::PointLightShader::SetLight( D3DXMATRIX & invViewProj, Light * light ) {
	if( light->pointTexture ) {
		pixelShaderTexProj->Bind();
		gpDevice->SetTexture( 3, light->pointTexture->cubeTexture );
	} else {
		pixelShader->Bind();
	}
	gpRenderer->SetPixelShaderFloat3( 8, g_camera->mGlobalTransform.getOrigin().m_floats );
	gpRenderer->SetPixelShaderMatrix( 0, &invViewProj );
	
	// position
    gpRenderer->SetPixelShaderFloat3( 5, light->GetRealPosition().elements ); 
	// color
    gpRenderer->SetPixelShaderFloat3( 6, light->GetColor().elements ); 
	 // range
    gpRenderer->SetPixelShaderFloat( 7, powf( light->GetRadius(), 4 ));
	// brightness
    gpRenderer->SetPixelShaderFloat( 9, g_hdrEnabled ? light->brightness : 1.0f ); 
}

DeferredRenderer::PointLightShader::~PointLightShader() {
    delete pixelShader;
}

////////////////////////////////////////////////////////////
// Ambient Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::AmbientLightShader::AmbientLightShader() {
    pixelShader = new PixelShader( "data/shaders/deferredAmbientLight.pso", true );
}

void DeferredRenderer::AmbientLightShader::Bind( ) {
    pixelShader->Bind();
    gpRenderer->SetPixelShaderFloat3( 0, g_ambientColor.elements );
}

DeferredRenderer::AmbientLightShader::~AmbientLightShader() {
    delete pixelShader;
}

////////////////////////////////////////////////////////////
// Spot Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::SpotLightShader::SpotLightShader( ) {
    pixelShader = new PixelShader( "data/shaders/deferredSpotLight.pso", true );
	pixelShaderShadows = new PixelShader( "data/shaders/deferredSpotLightShadows.pso", true );
}

void DeferredRenderer::SpotLightShader::SetLight( D3DXMATRIX & invViewProj, Light * lit ) {
	if( g_useSpotLightShadows ) {		
		pixelShaderShadows->Bind();		
	} else {
		pixelShader->Bind();
	}

	lit->spotTexture->Bind(3);
	lit->BuildSpotProjectionMatrixAndFrustum();
	gpRenderer->SetPixelShaderMatrix( 5, &lit->spotViewProjectionMatrix );
	gpRenderer->SetPixelShaderFloat3( 13, g_camera->mGlobalTransform.getOrigin().m_floats );
	gpRenderer->SetPixelShaderMatrix( 0, &invViewProj );

    btVector3 direction = ( lit->mGlobalTransform.getBasis() * btVector3( 0, 1, 0 )).normalize();
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
    // brightness
    gpRenderer->SetPixelShaderFloat( 17, (g_hdrEnabled ? lit->brightness : 1.0f) );
}

DeferredRenderer::SpotLightShader::~SpotLightShader() {
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
    GetD3DMatrixFromBulletTransform( lit->mGlobalTransform, world );
    D3DXMatrixMultiply( &world, &scale, &world );
    bvRenderer->Bind();
    D3DXMATRIX wvp;
    D3DXMatrixMultiply( &wvp, &world, &g_camera->viewProjection );
    bvRenderer->SetTransform( wvp );
    cone->DrawSubset( 0 );
}

void DeferredRenderer::EndFirstPassAndDoSecondPass() {
    OnEnd();

    if( mHDRShader && g_hdrEnabled ) {
        mHDRShader->SetAsRenderTarget();
    } else {
        if( g_fxaaEnabled ) {
            mFXAA->BeginDrawIntoTexture();
        } else {
            mGBuffer->BindBackSurfaceAsRT();
        }
    }

    gpDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
	
    if( g_camera->skybox ) {
        g_camera->skybox->Render( g_camera->mGlobalTransform.getOrigin() );
    }  

    mGBuffer->BindTextures();
	gpDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    mAmbientLightShader->Bind();
    mFullscreenQuad->Bind();
    mFullscreenQuad->Render();
	gpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	gpDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

    // Render point lights
    for( auto pLight : g_pointLightList ) {
		if( g_camera->frustum.IsSphereInside( pLight->GetRealPosition(), pLight->GetRadius() ) && pLight->IsVisible() ) {
			gpDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
			gpDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
			gpDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

			RenderIcosphereIntoStencilBuffer( pLight );

			mPointLightShader->SetLight( g_camera->invViewProjection, pLight );

			mFullscreenQuad->Bind();		

			gpDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
			gpDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
			gpDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );
			// quad render
			mFullscreenQuad->Render();
		}
    }

    // Render spot lights
    for( auto pLight : g_spotLightList ) {
		if( g_camera->frustum.IsSphereInside( pLight->GetRealPosition(), pLight->GetRadius() ) && pLight->IsVisible()  ) {
			if( g_useSpotLightShadows ) {
				IDirect3DSurface9 * prevSurface = nullptr;
				if( mHDRShader && g_hdrEnabled ) {
					prevSurface = mHDRShader->hdrSurface;
				} else if( g_fxaaEnabled ) {
					prevSurface = mFXAA->renderTarget;
				} else {
					prevSurface = mGBuffer->backSurface;
				}
				mSpotLightShadowMap->UnbindSpotShadowMap( 4 );

				gpDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
				gpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				gpDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

				mSpotLightShadowMap->RenderSpotShadowMap( prevSurface, 0, pLight );

				gpDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
				gpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				gpDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

				mSpotLightShadowMap->BindSpotShadowMap( 4 );
			}

			gpDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
			gpDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
			gpDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

			RenderConeIntoStencilBuffer( pLight );

			mSpotLightShader->SetLight( g_camera->invViewProjection, pLight );

			gpDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
			gpDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
			gpDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );

			mFullscreenQuad->Bind();
			mFullscreenQuad->Render();
		}
    }
    if( mHDRShader && g_hdrEnabled ) {
		gpDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
		gpDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
		gpDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
        mHDRShader->CalculateFrameLuminance( );
        if( g_fxaaEnabled ) {
            mHDRShader->DoToneMapping( mFXAA->renderTarget );
            mFXAA->DoAntialiasing( mFXAA->texture );
        } else {
            mHDRShader->DoToneMapping( mGBuffer->backSurface );
        }
    } else {
        if( g_fxaaEnabled ) {
            gpDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
            gpDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
			gpDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );
			gpDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
			gpDevice->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE );
			gpDevice->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
			gpDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
			gpDevice->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
            mFXAA->DoAntialiasing( mFXAA->texture );
			gpDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			gpDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			gpDevice->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			gpDevice->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			gpDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			gpDevice->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
        }
    }
}


void DeferredRenderer::SetSpotLightShadowMapSize( int size ) {
    if( size != mSpotLightShadowMap->iSize ) {
        if( mSpotLightShadowMap ) {
            delete mSpotLightShadowMap;
        }
        mSpotLightShadowMap = new SpotlightShadowMap( size );
    }
}
