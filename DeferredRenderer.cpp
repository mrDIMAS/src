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
#include "SceneFactory.h"
#include <random>

struct A8R8G8B8Pixel {
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct XYZNormalVertex {
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
};

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
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis( 100, 255 );

	mNoiseTextureSize = 64;
	Engine::I().GetDevice()->CreateTexture( mNoiseTextureSize, mNoiseTextureSize, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &mNoiseTexture, NULL ); 
	D3DLOCKED_RECT texRect;
	mNoiseTexture->LockRect( 0, &texRect, 0, D3DLOCK_DISCARD );
	A8R8G8B8Pixel * pixels = reinterpret_cast<A8R8G8B8Pixel*>( texRect.pBits );
	for( int i = 0; i < mNoiseTextureSize; i++ ) {
		for( int j = 0; j < mNoiseTextureSize; j++ ) {
			int index = i * mNoiseTextureSize + j;
			uint8_t rndVal = dis( gen );
			pixels[ index ].a = rndVal;
			pixels[ index ].r = rndVal;
			pixels[ index ].g = rndVal;
			pixels[ index ].b = rndVal;
		}
	}
	mNoiseTexture->UnlockRect( 0 );
}

DeferredRenderer::~DeferredRenderer() {
	OnLostDevice();
}

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

void DeferredRenderer::PointLightShader::SetLight( D3DXMATRIX & invViewProj, const shared_ptr<PointLight> & light, int noiseTexSize ) {
	if( light->GetPointTexture() ) {
		Engine::I().GetDevice()->SetTexture( 3, light->GetPointTexture()->mCubeTexture );
	} else {
		Log::Write( "Environment light cube texture is not set! ");
	}

	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		Engine::I().SetPixelShaderFloat3( 8, camera->GetPosition().elements );
	}
	Engine::I().SetPixelShaderMatrix( 0, &invViewProj );
	
	// position
    Engine::I().SetPixelShaderFloat3( 5, light->GetPosition().elements ); 
	// color
    Engine::I().SetPixelShaderFloat3( 6, light->GetColor().elements ); 
	 // range
	Engine::I().SetPixelShaderFloat( 7, light->GetRange() );
	// grey scale factor
    Engine::I().SetPixelShaderFloat3( 9, light->GetGrayscaleFactor(), 1.0f - light->GetGrayscaleFactor(), 0.0f ); 
	Engine::I().SetPixelShaderFloat( 10, noiseTexSize * frandom( 0.5f, 1.0f )); 
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

void DeferredRenderer::SpotLightShader::SetLight( D3DXMATRIX & invViewProj, const shared_ptr<SpotLight> & lit, int noiseTexSize ) {
	if( Engine::I().IsSpotLightShadowsEnabled() ) {		
		mPixelShaderShadows->Bind();		
	} else {
		mPixelShader->Bind();
	}

	lit->GetSpotTexture()->Bind(3);
	lit->BuildSpotProjectionMatrixAndFrustum();
	Engine::I().SetPixelShaderMatrix( 5, &lit->GetViewProjectionMatrix() );
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		Engine::I().SetPixelShaderFloat3( 13, camera->GetPosition().elements );
	}
	Engine::I().SetPixelShaderMatrix( 0, &invViewProj );

    btVector3 direction = ( lit->GetGlobalTransform().getBasis() * btVector3( 0, 1, 0 )).normalize();
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

	Engine::I().SetPixelShaderFloat( 18, noiseTexSize * frandom( 0.5f, 1.0f )); 
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
    mVertexDeclaration.Reset();
}

void DeferredRenderer::BoundingVolumeRenderingShader::Bind() {
    Engine::I().GetDevice()->SetVertexDeclaration( mVertexDeclaration );
}

void DeferredRenderer::BoundingVolumeRenderingShader::SetTransform( D3DXMATRIX & wvp ) {
    Engine::I().SetVertexShaderMatrix( 0, &wvp );
}

void DeferredRenderer::BoundingVolumeRenderingShader::OnLostDevice() {
	mVertexDeclaration.Reset();
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

void DeferredRenderer::RenderSphere( const shared_ptr<PointLight> & pLight, float scale /*= 1.0f */ ) {
    D3DXMATRIX world = SetUniformScaleTranslationMatrix( 1.5f * pLight->mRadius * scale, pLight->GetPosition() );
    bvRenderer->Bind();
    D3DXMATRIX wvp;
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		D3DXMatrixMultiply( &wvp, &world, &camera->mViewProjection );
	}
    bvRenderer->SetTransform( wvp );
	mBoundingSphere->DrawSubset( 0 );
}

void DeferredRenderer::RenderStar( shared_ptr<PointLight> pLight, float scale ) {
	D3DXMATRIX world = SetUniformScaleTranslationMatrix( 1.25f * pLight->mRadius * scale,  pLight->GetPosition() );
	bvRenderer->Bind();
	D3DXMATRIX wvp;
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		D3DXMatrixMultiply( &wvp, &world, &camera->mViewProjection );
	}
	bvRenderer->SetTransform( wvp );
	mBoundingStar->DrawSubset( 0 );
}

void DeferredRenderer::RenderCone( shared_ptr<SpotLight> lit ) {
    float height = lit->GetRange() * 2.5;
    float radius = height * sinf( ( lit->GetOuterAngle() * 0.75f ) * SIMD_PI / 180.0f );
    D3DXMATRIX scale, world, wvp;
    D3DXMatrixScaling( &scale, radius, height, radius );
    world = lit->GetWorldMatrix();
    D3DXMatrixMultiply( &world, &scale, &world );
    bvRenderer->Bind();
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		D3DXMatrixMultiply( &wvp, &world, &camera->mViewProjection );
	}
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

	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		if( camera->mSkybox ) {
			if( Engine::I().IsHDREnabled() ) {
				Engine::I().GetDevice()->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
				Engine::I().GetDevice()->SetSamplerState( 2, D3DSAMP_SRGBTEXTURE, FALSE );
			}
			Engine::I().SetZWriteEnabled( false );
			Engine::I().GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			Engine::I().SetAlphaBlendEnabled( false );
			Engine::I().SetStencilEnabled( false );
			mSkyboxShader->Bind( camera->GetPosition() );
			camera->mSkybox->Render( );		
			if( Engine::I().IsHDREnabled() ) {
				Engine::I().GetDevice()->SetRenderState( D3DRS_SRGBWRITEENABLE, TRUE );
				Engine::I().GetDevice()->SetSamplerState( 2, D3DSAMP_SRGBTEXTURE, TRUE );
			}
		}  
		mGBuffer->BindTextures();
		mFullscreenQuad->Bind();
	
	
		Engine::I().SetZWriteEnabled( false );

		// begin occlusion queries
		Engine::I().SetAlphaBlendEnabled( false );
		Engine::I().SetStencilEnabled( false );
		Engine::I().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
		Engine::I().GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	

		int countInFrustum = 0;

		auto & pointLights = SceneFactory::GetPointLightList();
		for( auto & lWeak : pointLights ) {
			shared_ptr<PointLight> & pLight = lWeak.lock();
			if( pLight ) {
				if( pLight->mQueryDone ) {
					if( camera->mFrustum.IsSphereInside( pLight->GetPosition(), pLight->GetRange() ) && pLight->IsVisible() ) {
						bool found = false;
						for( auto & lit : camera->GetNearestPathPoint()->GetListOfVisibleLights() ) {
							if( lit.lock() == pLight ) {
								found = true;
							}
						}
						if( !found ) {
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
		}

		Engine::I().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
		Engine::I().SetStencilEnabled( true );
		Engine::I().SetAlphaBlendEnabled( true );
	
		for( auto & lWeak : pointLights ) {
			shared_ptr<PointLight> & pLight = lWeak.lock();
			if( pLight ) {
				bool inFrustum = camera->mFrustum.IsSphereInside( pLight->GetPosition(), pLight->GetRange() );
				if( pLight->IsVisible() && inFrustum ) {
					bool found = false;
					for( auto & lit : camera->GetNearestPathPoint()->GetListOfVisibleLights() ) {
						if( lit.lock() == pLight ) {
							found = true;
							break;
						}
					}
					DWORD pixelsVisible = 0;
					if( pLight->mInFrustum  && !pLight->mQueryDone ) {
						if( !found ) {
							HRESULT result = pLight->pQuery->GetData( &pixelsVisible, sizeof( pixelsVisible ), D3DGETDATA_FLUSH ) ;
							if( result == S_OK ) {
								pLight->mQueryDone = true;
								if( pixelsVisible > 0 ) {				
									// add light to light list of nearest path point of camera									
									camera->GetNearestPathPoint()->GetListOfVisibleLights().push_back( pLight );								
								}
							}
						}
					}
				}
			}
		}

		// Render point lights
		Engine::I().GetDevice()->SetTexture( 4, mNoiseTexture );
		mFullscreenQuad->mVertexShader->Bind();	

		for( auto lWeak : camera->GetNearestPathPoint()->GetListOfVisibleLights() ) {
			shared_ptr<PointLight> & pLight = lWeak.lock();
			if( pLight ) {
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

						mPointLightShader->SetLight( camera->invViewProjection, pLight, mNoiseTextureSize );

						RenderSphere( pLight );

						mFullscreenQuad->BindNoShader();

						Engine::I().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
						Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
						Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );

						mFullscreenQuad->Render();			
					}		
				}
			}
		}
	
		// Render spot lights
		Engine::I().GetDevice()->SetTexture( 5, mNoiseTexture );
		auto & spotLights = SceneFactory::GetSpotLightList();
		for( auto & lWeak : spotLights ) {
			shared_ptr<SpotLight> & pLight = lWeak.lock();
			if( pLight ) {
				if( camera->mFrustum.IsSphereInside( pLight->GetPosition(), pLight->GetRange() ) && pLight->IsVisible()  ) {
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

					mSpotLightShader->SetLight( camera->invViewProjection, pLight, mNoiseTextureSize );

					Engine::I().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
					Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
					Engine::I().GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );


					mFullscreenQuad->Bind();
					mFullscreenQuad->Render();
				}
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
	}

	// Postprocessing
	Engine::I().SetStencilEnabled( false );
	Engine::I().SetZWriteEnabled( false );
	Engine::I().SetZEnabled( true );
	mPostprocessing->RenderMask();
	Engine::I().SetZEnabled( false );
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
	mBoundingStar.Reset();
	mBoundingSphere.Reset();
	mBoundingCone.Reset();	
	mHDRFrameSurface.Reset();
	mHDRFrame.Reset();
	for( int i = 0; i < 2; i++ ) {
		mFrameSurface[i].Reset();
		mFrame[i].Reset();
	}
}

void DeferredRenderer::OnResetDevice() {
	CreateBoundingVolumes();
}

void DeferredRenderer::SkyboxShader::Bind( const ruVector3 & position ) {
	mPixelShader->Bind();
	mVertexShader->Bind();
	D3DXMATRIX matrix;
	D3DXMatrixTranslation( &matrix, position.x, position.y, position.z );
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		D3DXMatrixMultiply( &matrix, &matrix, &camera->mViewProjection );
	}

	Engine::I().SetVertexShaderMatrix( 0, &matrix );
}

DeferredRenderer::SkyboxShader::SkyboxShader() {
	mVertexShader = unique_ptr<VertexShader>( new VertexShader( "data/shaders/skybox.vso" ));
	mPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/skybox.pso" ));
}

DeferredRenderer::SkyboxShader::~SkyboxShader() {
}

