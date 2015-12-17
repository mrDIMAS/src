/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#include "Precompiled.h"
#include "DeferredRenderer.h"
#include "Light.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Camera.h"
#include "Skybox.h"
#include "Utility.h"
#include "Engine.h"

DeferredRenderer::DeferredRenderer() {

	// Create render targets 
	int width = Engine::I().GetResolutionWidth();
	int height = Engine::I().GetResolutionHeight();

	D3DXCreateTexture( Engine::I().GetDevice(), width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT, &mHDRFrame );
	mHDRFrame->GetSurfaceLevel( 0, &mHDRFrameSurface );

	for( int i = 0; i < 2; i++ ) {
		D3DXCreateTexture( Engine::I().GetDevice(), width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &mFrame[i] );
		mFrame[i]->GetSurfaceLevel( 0, &mFrameSurface[i] );
	}

	mBackBufferSurface = Engine::I().GetBackBuffer();

    mFullscreenQuad = unique_ptr<EffectsQuad>( new EffectsQuad() );
    CreateBoundingVolumes();
    mFXAA = unique_ptr<FXAA>( new FXAA );
    mGBuffer = unique_ptr<GBuffer>( new GBuffer );
    mSpotLightShader = unique_ptr<SpotLightShader>( new SpotLightShader );
    mAmbientLightShader = unique_ptr<AmbientLightShader>( new AmbientLightShader );
    mPointLightShader = unique_ptr<PointLightShader>( new PointLightShader );
    bvRenderer = unique_ptr<BoundingVolumeRenderingShader>( new BoundingVolumeRenderingShader );
    mSpotLightShadowMap = unique_ptr<SpotlightShadowMap>( new SpotlightShadowMap );
	mSkyboxShader = unique_ptr<SkyboxShader>( new SkyboxShader );
	mPostprocessing = unique_ptr<Postprocessing>( new Postprocessing );
    if( Engine::I().IsTextureFormatOk( D3DFMT_A16B16G16R16 )) {
        mHDRShader = unique_ptr<HDRShader>( new HDRShader );
    };
}

DeferredRenderer::~DeferredRenderer() {
	OnLostDevice();
}

struct XYZNormalVertex {
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
};

void DeferredRenderer::CreateBoundingVolumes() {
    int quality = 6;

	D3DVERTEXELEMENT9 vd[ ] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	XYZNormalVertex * data;

    D3DXCreateSphere( Engine::I().GetDevice(), 1.0, quality, quality, &mBoundingSphere, 0 );
	mBoundingSphere->UpdateSemantics( vd );

    D3DXCreateCylinder( Engine::I().GetDevice(), 0.0f, 1.0f, 1.0f, quality, quality, &mBoundingCone, 0 );
    // rotate cylinder on 90 degrees
    mBoundingCone->LockVertexBuffer( 0, reinterpret_cast<void**>( &data ));
    D3DXMATRIX tran, rot90, transform;
    D3DXMatrixTranslation( &tran, 0, -0.5, 0 );
    D3DXMatrixRotationAxis( &rot90, &D3DXVECTOR3( 1, 0, 0 ), SIMD_HALF_PI ); 
    D3DXMatrixMultiply( &transform, &rot90, &tran );
    for( int i = 0; i < mBoundingCone->GetNumVertices(); i++ ) {
        XYZNormalVertex * v = &data[ i ];
        D3DXVec3TransformCoord( &v->p, &v->p, &transform );
    }
    mBoundingCone->UnlockVertexBuffer();
	mBoundingCone->UpdateSemantics( vd );

	D3DXCreateSphere( Engine::I().GetDevice(), 1.0, quality, quality, &mBoundingStar, 0 );
	
	mBoundingStar->LockVertexBuffer( 0, reinterpret_cast<void**>( &data ));
	int n = 0;
	for( int i = 0; i < mBoundingStar->GetNumVertices(); i++ ) {
		XYZNormalVertex * v = &data[ i ];
		n++;
		if( n == 5 ) {
			v->p.x = 0;
			v->p.y = 0;
			v->p.z = 0;
			n = 0;
		}
	}
	mBoundingStar->UnlockVertexBuffer();
	mBoundingStar->UpdateSemantics( vd );
}

////////////////////////////////////////////////////////////
// Point Light Subclass
////////////////////////////////////////////////////////////
DeferredRenderer::PointLightShader::PointLightShader() {
    mPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredPointLight.pso" ));
    mPixelShaderTexProj = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredPointLightTexProj.pso" ));
}

void DeferredRenderer::PointLightShader::SetLight( D3DXMATRIX & invViewProj, PointLight * light ) {
	// HACK HACK HACK
	/**
	bool found = false;
	auto pointIter = find( Light::msPointLightList.begin(), Light::msPointLightList.end(), light );
	if( pointIter != Light::msPointLightList.end() ) {
		found = true;
	}

	if( !found ) {
		auto spotIter = find( Light::msSpotLightList.begin(), Light::msSpotLightList.end(), light );
		if( spotIter == Light::msSpotLightList.end() ) {
			return;
		}
	}*/

	if( light->GetPointTexture() ) {
		Engine::I().GetDevice()->SetTexture( 3, light->GetPointTexture()->cubeTexture );
	} else {
		Log::Write( "Environment light cube texture is not set! ");
	}

	Engine::I().SetPixelShaderFloat3( 8, Camera::msCurrentCamera->mGlobalTransform.getOrigin().m_floats );
	Engine::I().SetPixelShaderMatrix( 0, &invViewProj );
	
	// position
    Engine::I().SetPixelShaderFloat3( 5, light->GetPosition().elements ); 
	// color
    Engine::I().SetPixelShaderFloat3( 6, light->GetColor().elements ); 
	 // range
	Engine::I().SetPixelShaderFloat( 7, light->GetRange() );
	// grey scale factor
    Engine::I().SetPixelShaderFloat3( 9, light->GetGrayscaleFactor(), 1.0f - light->GetGrayscaleFactor(), 0.0f ); 

	Engine::I().SetPixelShaderFloat( 10, frandom( 0.2, 1 ) ); 
}

DeferredRenderer::PointLightShader::~PointLightShader() {
}

////////////////////////////////////////////////////////////
// Ambient Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::AmbientLightShader::AmbientLightShader() {
    pixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredAmbientLight.pso" ));
}

void DeferredRenderer::AmbientLightShader::Bind( ) {
    pixelShader->Bind();
    Engine::I().SetPixelShaderFloat3( 0, Engine::I().GetAmbientColor().elements );
}

DeferredRenderer::AmbientLightShader::~AmbientLightShader() {
}

////////////////////////////////////////////////////////////
// Spot Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::SpotLightShader::SpotLightShader( ) {
    mPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredSpotLight.pso" ));
	mPixelShaderShadows = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredSpotLightShadows.pso" ));
}

void DeferredRenderer::SpotLightShader::SetLight( D3DXMATRIX & invViewProj, SpotLight * lit ) {
	if( Engine::I().IsSpotLightShadowsEnabled() ) {		
		mPixelShaderShadows->Bind();		
	} else {
		mPixelShader->Bind();
	}

	lit->GetSpotTexture()->Bind(3);
	lit->BuildSpotProjectionMatrixAndFrustum();
	Engine::I().SetPixelShaderMatrix( 5, &lit->GetViewProjectionMatrix() );
	Engine::I().SetPixelShaderFloat3( 13, Camera::msCurrentCamera->mGlobalTransform.getOrigin().m_floats );
	Engine::I().SetPixelShaderMatrix( 0, &invViewProj );

    btVector3 direction = ( lit->mGlobalTransform.getBasis() * btVector3( 0, 1, 0 )).normalize();
    // position
    Engine::I().SetPixelShaderFloat3( 10, lit->GetPosition().elements );
    // range
    Engine::I().SetPixelShaderFloat( 14, lit->GetRange() );
    // color
    Engine::I().SetPixelShaderFloat3( 11, lit->GetColor().elements );
    // inner angle
    Engine::I().SetPixelShaderFloat( 15, lit->GetCosHalfInnerAngle() );
    // outer angle
    Engine::I().SetPixelShaderFloat( 16, lit->GetCosHalfOuterAngle() );
    // direction
    Engine::I().SetPixelShaderFloat3( 12, direction.m_floats );

	Engine::I().SetPixelShaderFloat3( 17, lit->GetGrayscaleFactor(), 1.0f - lit->GetGrayscaleFactor(), 0.0f );

	Engine::I().SetPixelShaderFloat( 18, frandom( 0.2, 1 ) ); 
}

DeferredRenderer::SpotLightShader::~SpotLightShader() {
}

//////////////////////////////////////////////////////////////////////////
// Bounding volume rendering shader
DeferredRenderer::BoundingVolumeRenderingShader::BoundingVolumeRenderingShader() {
    mPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/boundingVolume.pso" ));
    OnResetDevice();
}

DeferredRenderer::BoundingVolumeRenderingShader::~BoundingVolumeRenderingShader() {
    mVertexDeclaration->Release();
}

void DeferredRenderer::BoundingVolumeRenderingShader::Bind() {
    Engine::I().GetDevice()->SetVertexDeclaration( mVertexDeclaration );
}

void DeferredRenderer::BoundingVolumeRenderingShader::SetTransform( D3DXMATRIX & wvp ) {
    Engine::I().SetVertexShaderMatrix( 0, &wvp );
}

void DeferredRenderer::BoundingVolumeRenderingShader::OnLostDevice() {
	mVertexDeclaration->Release();
}

void DeferredRenderer::BoundingVolumeRenderingShader::OnResetDevice() {
	D3DVERTEXELEMENT9 vd[ ] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,  0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	Engine::I().GetDevice()->CreateVertexDeclaration( vd, &mVertexDeclaration ) ;
}

D3DXMATRIX SetUniformScaleTranslationMatrix( float s, const ruVector3 & p ) {
	return D3DXMATRIX ( s, 0.0f, 0.0f, 0.0f,
					    0.0f, s, 0.0f, 0.0f,
					    0.0f, 0.0f, s, 0.0f,
					    p.x, p.y, p.z, 1.0f );
}

void DeferredRenderer::RenderSphere( PointLight * pLight, float scale ) {
    D3DXMATRIX world = SetUniformScaleTranslationMatrix( 1.5f * pLight->mRadius * scale, pLight->GetPosition() );
    bvRenderer->Bind();
    D3DXMATRIX wvp;
    D3DXMatrixMultiply( &wvp, &world, &Camera::msCurrentCamera->mViewProjection );
    bvRenderer->SetTransform( wvp );
	mBoundingSphere->DrawSubset( 0 );
}

void DeferredRenderer::RenderStar( PointLight * pLight, float scale ) {
	D3DXMATRIX world = SetUniformScaleTranslationMatrix( 1.25f * pLight->mRadius * scale,  pLight->GetPosition() );
	bvRenderer->Bind();
	D3DXMATRIX wvp;
	D3DXMatrixMultiply( &wvp, &world, &Camera::msCurrentCamera->mViewProjection );
	bvRenderer->SetTransform( wvp );
	mBoundingStar->DrawSubset( 0 );
}

void DeferredRenderer::RenderCone( SpotLight * lit ) {
    float height = lit->GetRange() * 2.5;
    float radius = height * sinf( ( lit->GetOuterAngle() * 0.75f ) * SIMD_PI / 180.0f );
    D3DXMATRIX scale, world, wvp;
    D3DXMatrixScaling( &scale, radius, height, radius );
    GetD3DMatrixFromBulletTransform( lit->mGlobalTransform, world );
    D3DXMatrixMultiply( &world, &scale, &world );
    bvRenderer->Bind();
    D3DXMatrixMultiply( &wvp, &world, &Camera::msCurrentCamera->mViewProjection );
    bvRenderer->SetTransform( wvp );
    mBoundingCone->DrawSubset( 0 );
}

void DeferredRenderer::DoLightingAndPostProcessing() {
    OnEnd();

	// select render target 
	IDirect3DSurface9 * renderTarget = mFrameSurface[0];

	if( mHDRShader && Engine::I().IsHDREnabled() ) {
		renderTarget = mHDRFrameSurface;
	}

	Engine::I().GetDevice()->SetRenderTarget( 0, renderTarget );
    Engine::I().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
	
	mGBuffer->BindTextures();

	Engine::I().SetZEnabled( true );

	mAmbientLightShader->Bind();
	mFullscreenQuad->Bind();
	mFullscreenQuad->Render();

	if( Camera::msCurrentCamera ) {
		if( Camera::msCurrentCamera->mSkybox ) {
			if( Engine::I().IsHDREnabled() ) {
				Engine::I().GetDevice()->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
				Engine::I().GetDevice()->SetSamplerState( 2, D3DSAMP_SRGBTEXTURE, FALSE );
			}
			Engine::I().SetZWriteEnabled( false );
			Engine::I().GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			Engine::I().SetAlphaBlendEnabled( false );
			Engine::I().SetStencilEnabled( false );
			mSkyboxShader->Bind( Camera::msCurrentCamera->mGlobalTransform.getOrigin() );
			Camera::msCurrentCamera->mSkybox->Render( );		
			if( Engine::I().IsHDREnabled() ) {
				Engine::I().GetDevice()->SetRenderState( D3DRS_SRGBWRITEENABLE, TRUE );
				Engine::I().GetDevice()->SetSamplerState( 2, D3DSAMP_SRGBTEXTURE, TRUE );
			}
		}  
		mGBuffer->BindTextures();
		mFullscreenQuad->Bind();
	}
	
	Engine::I().SetZWriteEnabled( false );

	// begin occlusion queries
	Engine::I().SetAlphaBlendEnabled( false );
	Engine::I().SetStencilEnabled( false );
	Engine::I().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
	Engine::I().GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	

	int countInFrustum = 0;
	for( auto pLight : PointLight::msPointLightList ) {
		if( pLight->mQueryDone ) {
			if( Camera::msCurrentCamera->mFrustum.IsSphereInside( pLight->GetPosition(), pLight->GetRange() ) && pLight->IsVisible() ) {
				auto iter = find( Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.begin(), Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.end(), pLight );
				if( iter == Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.end() ) {
					pLight->pQuery->Issue( D3DISSUE_BEGIN );
					RenderStar( pLight );
					pLight->pQuery->Issue( D3DISSUE_END );
				}
				pLight->mInFrustum = true;
				countInFrustum++;
				pLight->mQueryDone = false;
			} else {
				pLight->mInFrustum = false;
			}
		}
	}

	Engine::I().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
	Engine::I().SetStencilEnabled( true );
	Engine::I().SetAlphaBlendEnabled( true );
	
	for( auto pLight : PointLight::msPointLightList ) {
		bool inFrustum = Camera::msCurrentCamera->mFrustum.IsSphereInside( pLight->GetPosition(), pLight->GetRange() );
		if( pLight->IsVisible() && inFrustum ) {
			auto iter = find( Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.begin(), Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.end(), pLight );			
			DWORD pixelsVisible = 0;
			if( pLight->mInFrustum  && !pLight->mQueryDone ) {
				if( iter == Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.end() ) {
					HRESULT result = pLight->pQuery->GetData( &pixelsVisible, sizeof( pixelsVisible ), D3DGETDATA_FLUSH ) ;
					if( result == S_OK ) {
						pLight->mQueryDone = true;
						if( pixelsVisible > 0 ) {				
							// add light to light list of nearest path point of camera									
							Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.push_back( pLight );								
						}
					}
				}
			}
		}
	}

	// Render point lights
	mFullscreenQuad->vertexShader->Bind();	

    for( auto pLight : Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList ) {
		if( pLight->IsVisible() ) {
			if( pLight->mInFrustum  ) {
				if( pLight->GetPointTexture() ) {
					mPointLightShader->mPixelShaderTexProj->Bind();
				} else {
					mPointLightShader->mPixelShader->Bind();
				}

				Engine::I().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
				Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
				Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );			

				Engine::I().SetZEnabled( true );
						
				mPointLightShader->SetLight( Camera::msCurrentCamera->invViewProjection, pLight );

				RenderSphere( pLight );

				mFullscreenQuad->BindNoShader();

				Engine::I().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
				Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
				Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );

				mFullscreenQuad->Render();			
			}		
		}
    }
	
    // Render spot lights
    for( auto pLight : SpotLight::msSpotLightList ) {
		if( Camera::msCurrentCamera->mFrustum.IsSphereInside( pLight->GetPosition(), pLight->GetRange() ) && pLight->IsVisible()  ) {
			if( Engine::I().IsSpotLightShadowsEnabled() ) {

				IDirect3DSurface9 * prevSurface = mFrameSurface[0];
				if( mHDRShader && Engine::I().IsHDREnabled() ) {
					prevSurface = mHDRFrameSurface;
				}

				mSpotLightShadowMap->UnbindSpotShadowMap( 4 );

				Engine::I().SetStencilEnabled( false );
				Engine::I().SetAlphaBlendEnabled( false );
				Engine::I().SetZWriteEnabled( true );

				mSpotLightShadowMap->RenderSpotShadowMap( prevSurface, 0, pLight );

				Engine::I().SetStencilEnabled( true );
				Engine::I().SetAlphaBlendEnabled( true );
				Engine::I().SetZWriteEnabled( false );

				mSpotLightShadowMap->BindSpotShadowMap( 4 );
			}

			Engine::I().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
			Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
			Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

			RenderCone( pLight );

			mSpotLightShader->SetLight( Camera::msCurrentCamera->invViewProjection, pLight );

			Engine::I().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
			Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
			Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );


			mFullscreenQuad->Bind();
			mFullscreenQuad->Render();
		}
    }
	
    if( mHDRShader && Engine::I().IsHDREnabled() ) {
		Engine::I().GetDevice()->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
		Engine::I().SetStencilEnabled( false );
		Engine::I().SetZEnabled( false );
		Engine::I().SetAlphaBlendEnabled( false );

		Engine::I().SetGenericSamplersFiltration( D3DTEXF_POINT, true );

        mHDRShader->CalculateFrameLuminance( mHDRFrame );
        if( Engine::I().IsFXAAEnabled() ) {
            mHDRShader->DoToneMapping( mFrameSurface[0], mHDRFrame );
            mFXAA->DoAntialiasing( mFrameSurface[1], mFrame[0] );
        } else {
            mHDRShader->DoToneMapping( mFrameSurface[1], mHDRFrame );
        }
    } else {
        if( Engine::I().IsFXAAEnabled() ) {
            Engine::I().SetStencilEnabled( false );
            Engine::I().SetZEnabled( false );

			Engine::I().SetGenericSamplersFiltration( D3DTEXF_POINT, true );

            mFXAA->DoAntialiasing( mFrameSurface[1], mFrame[0] );
        }
    }

	// Postprocessing
	Engine::I().SetZWriteEnabled( false );
	mPostprocessing->RenderMask();
	Engine::I().SetZWriteEnabled( true );

	if( Engine::I().IsFXAAEnabled() || Engine::I().IsHDREnabled() ) {
		mPostprocessing->DoPostprocessing( mBackBufferSurface, mFrame[1] );
	} else {
		mPostprocessing->DoPostprocessing( mBackBufferSurface, mFrame[0] );
	}
		
	if( Engine::I().IsAnisotropicFilteringEnabled() ) {
		Engine::I().SetGenericSamplersFiltration( D3DTEXF_ANISOTROPIC, false );
	} else {
		Engine::I().SetGenericSamplersFiltration( D3DTEXF_LINEAR, false );
	}

	Engine::I().SetAlphaBlendEnabled( true );
}


void DeferredRenderer::SetSpotLightShadowMapSize( int size ) {
    if( size != mSpotLightShadowMap->iSize ) {
        if( mSpotLightShadowMap ) {
            mSpotLightShadowMap.reset();
        }
        mSpotLightShadowMap.reset( new SpotlightShadowMap( (float)size ));
    }
}

void DeferredRenderer::OnLostDevice() {
	mBoundingStar->Release();
	mBoundingSphere->Release();
	mBoundingCone->Release();

	mHDRFrameSurface->Release();
	mHDRFrame->Release();

	for( int i = 0; i < 2; i++ ) {
		mFrameSurface[i]->Release();
		mFrame[i]->Release();
	}
}

void DeferredRenderer::OnResetDevice() {
	CreateBoundingVolumes();
}

void DeferredRenderer::SkyboxShader::Bind( const btVector3 & position ) {
	mPixelShader->Bind();
	mVertexShader->Bind();
	D3DXMATRIX matrix;
	D3DXMatrixTranslation( &matrix, position.x(), position.y(), position.z() );
	D3DXMatrixMultiply( &matrix, &matrix, &Camera::msCurrentCamera->mViewProjection );

	Engine::I().SetVertexShaderMatrix( 0, &matrix );
}

DeferredRenderer::SkyboxShader::SkyboxShader() {
	mVertexShader = unique_ptr<VertexShader>( new VertexShader( "data/shaders/skybox.vso" ));
	mPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/skybox.pso" ));
}

DeferredRenderer::SkyboxShader::~SkyboxShader() {
}

