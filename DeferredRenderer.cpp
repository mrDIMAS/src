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
    mFullscreenQuad = make_shared<EffectsQuad>();
    CreateBoundingVolumes();
    mFXAA = make_shared<FXAA>();
    mGBuffer = make_shared<GBuffer>();
    mSpotLightShader = make_shared<SpotLightShader>();
    mAmbientLightShader = make_shared<AmbientLightShader>();
    mPointLightShader = make_shared<PointLightShader>();
    bvRenderer = make_shared<BoundingVolumeRenderingShader>();
    mSpotLightShadowMap = make_shared<SpotlightShadowMap>();
	mSkyboxShader = make_shared<SkyboxShader>();
    // check support of floating-point textures first
    if( Engine::Instance().IsTextureFormatOk( D3DFMT_A16B16G16R16 )) {
        mHDRShader = make_shared<HDRShader>();
    } else {
        mHDRShader = nullptr;
    }
}

DeferredRenderer::~DeferredRenderer() {
	OnLostDevice();
}

GBuffer * DeferredRenderer::GetGBuffer() {
    return mGBuffer.get();
}

struct XYZNormalVertex {
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
};

void DeferredRenderer::CreateBoundingVolumes() {
	ID3DXMesh * temp = nullptr;
    int quality = 6;

	D3DVERTEXELEMENT9 vd[ ] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	XYZNormalVertex * data;

    D3DXCreateSphere( Engine::Instance().GetDevice(), 1.0, quality, quality, &mBoundingSphere, 0 );
	mBoundingSphere->UpdateSemantics( vd );

    D3DXCreateCylinder( Engine::Instance().GetDevice(), 0.0f, 1.0f, 1.0f, quality, quality, &mBoundingCone, 0 );
    // rotate cylinder on 90 degrees
    mBoundingCone->LockVertexBuffer( 0, (void**)&data );
    D3DXMATRIX tran;
    D3DXMatrixTranslation( &tran, 0, -0.5, 0 );
    D3DXMATRIX rot90;
    D3DXMatrixRotationAxis( &rot90, &D3DXVECTOR3( 1, 0, 0 ), SIMD_HALF_PI );
    D3DXMATRIX transform;
    D3DXMatrixMultiply( &transform, &rot90, &tran );
    for( int i = 0; i < mBoundingCone->GetNumVertices(); i++ ) {
        XYZNormalVertex * v = &data[ i ];
        D3DXVec3TransformCoord( &v->p, &v->p, &transform );
    }
    mBoundingCone->UnlockVertexBuffer();
	mBoundingCone->UpdateSemantics( vd );

	D3DXCreateSphere( Engine::Instance().GetDevice(), 1.0, quality, quality, &mBoundingStar, 0 );
	
	mBoundingStar->LockVertexBuffer( 0, (void**)&data );
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
    pixelShader = new PixelShader( "data/shaders/deferredPointLight.pso" );
    pixelShaderTexProj = new PixelShader( "data/shaders/deferredPointLightTexProj.pso" );
}

void DeferredRenderer::PointLightShader::SetLight( D3DXMATRIX & invViewProj, Light * light ) {
	// HACK HACK HACK
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
	}

	if( light->pointTexture ) {
		Engine::Instance().GetDevice()->SetTexture( 3, light->pointTexture->cubeTexture );
	} else {
		Log::Write( "Environment light cube texture is not set! ");
	}

	Engine::Instance().SetPixelShaderFloat3( 8, Camera::msCurrentCamera->mGlobalTransform.getOrigin().m_floats );
	Engine::Instance().SetPixelShaderMatrix( 0, &invViewProj );
	
	// position
    Engine::Instance().SetPixelShaderFloat3( 5, light->GetRealPosition().elements ); 
	// color
    Engine::Instance().SetPixelShaderFloat3( 6, light->GetColor().elements ); 
	 // range
	Engine::Instance().SetPixelShaderFloat( 7, light->GetRadius() );
	// brightness
    Engine::Instance().SetPixelShaderFloat( 9, Engine::Instance().IsHDREnabled() ? light->brightness : 1.0f ); 
}

DeferredRenderer::PointLightShader::~PointLightShader() {
    delete pixelShader;
	delete pixelShaderTexProj;
}

////////////////////////////////////////////////////////////
// Ambient Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::AmbientLightShader::AmbientLightShader() {
    pixelShader = new PixelShader( "data/shaders/deferredAmbientLight.pso" );
}

void DeferredRenderer::AmbientLightShader::Bind( ) {
    pixelShader->Bind();
    Engine::Instance().SetPixelShaderFloat3( 0, Engine::Instance().GetAmbientColor().elements );
}

DeferredRenderer::AmbientLightShader::~AmbientLightShader() {
    delete pixelShader;
}

////////////////////////////////////////////////////////////
// Spot Light Subclass
////////////////////////////////////////////////////////////

DeferredRenderer::SpotLightShader::SpotLightShader( ) {
    pixelShader = new PixelShader( "data/shaders/deferredSpotLight.pso" );
	pixelShaderShadows = new PixelShader( "data/shaders/deferredSpotLightShadows.pso" );
}

void DeferredRenderer::SpotLightShader::SetLight( D3DXMATRIX & invViewProj, Light * lit ) {
	if( Engine::Instance().IsSpotLightShadowsEnabled() ) {		
		pixelShaderShadows->Bind();		
	} else {
		pixelShader->Bind();
	}

	lit->spotTexture->Bind(3);
	lit->BuildSpotProjectionMatrixAndFrustum();
	Engine::Instance().SetPixelShaderMatrix( 5, &lit->spotViewProjectionMatrix );
	Engine::Instance().SetPixelShaderFloat3( 13, Camera::msCurrentCamera->mGlobalTransform.getOrigin().m_floats );
	Engine::Instance().SetPixelShaderMatrix( 0, &invViewProj );

    btVector3 direction = ( lit->mGlobalTransform.getBasis() * btVector3( 0, 1, 0 )).normalize();
    // position
    Engine::Instance().SetPixelShaderFloat3( 10, lit->GetRealPosition().elements );
    // range
    Engine::Instance().SetPixelShaderFloat( 14, lit->GetRadius() );
    // color
    Engine::Instance().SetPixelShaderFloat3( 11, lit->GetColor().elements );
    // inner angle
    Engine::Instance().SetPixelShaderFloat( 15, lit->GetCosHalfInnerAngle() );
    // outer angle
    Engine::Instance().SetPixelShaderFloat( 16, lit->GetCosHalfOuterAngle() );
    // direction
    Engine::Instance().SetPixelShaderFloat3( 12, direction.m_floats );
}

DeferredRenderer::SpotLightShader::~SpotLightShader() {
    delete pixelShader;
	delete pixelShaderShadows;
}

//////////////////////////////////////////////////////////////////////////
// Bounding volume rendering shader
//
// Bounding volume for a light can be a sphere for point light
// a oriented cone for a spot light
DeferredRenderer::BoundingVolumeRenderingShader::BoundingVolumeRenderingShader() {
    //vs = new VertexShader( "data/shaders/boundingVolume.vso", true );
    ps = new PixelShader( "data/shaders/boundingVolume.pso" );
    OnResetDevice();
}

DeferredRenderer::BoundingVolumeRenderingShader::~BoundingVolumeRenderingShader() {
    delete ps;
    //delete vs;
    vertexDeclaration->Release();
}

void DeferredRenderer::BoundingVolumeRenderingShader::Bind() {
    Engine::Instance().GetDevice()->SetVertexDeclaration( vertexDeclaration );
}

void DeferredRenderer::BoundingVolumeRenderingShader::SetTransform( D3DXMATRIX & wvp ) {
    Engine::Instance().SetVertexShaderMatrix( 0, &wvp );
}

void DeferredRenderer::BoundingVolumeRenderingShader::OnLostDevice()
{
	vertexDeclaration->Release();
}

void DeferredRenderer::BoundingVolumeRenderingShader::OnResetDevice()
{
	D3DVERTEXELEMENT9 vd[ ] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,  0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		//{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		D3DDECL_END()
	};
	Engine::Instance().GetDevice()->CreateVertexDeclaration( vd, &vertexDeclaration ) ;
}

void DeferredRenderer::RenderSphere( Light * pLight, float scale ) {
    ruVector3 realPosition = pLight->GetRealPosition();
    float scl = 3.0f * pLight->radius * scale;
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
    D3DXMatrixMultiply( &wvp, &world, &Camera::msCurrentCamera->mViewProjection );
    bvRenderer->SetTransform( wvp );

	mBoundingSphere->DrawSubset( 0 );
}

void DeferredRenderer::RenderStar( Light * pLight, float scale ) {
	ruVector3 realPosition = pLight->GetRealPosition();
	float scl = 2.75f * pLight->radius * scale;
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
	D3DXMatrixMultiply( &wvp, &world, &Camera::msCurrentCamera->mViewProjection );
	bvRenderer->SetTransform( wvp );

	mBoundingStar->DrawSubset( 0 );
}

void DeferredRenderer::RenderConeIntoStencilBuffer( Light * lit ) {
    float height = lit->GetRadius() * 2.5;
    float radius = height * sinf( ( lit->GetOuterAngle() * 0.75f ) * SIMD_PI / 180.0f );
    D3DXMATRIX scale;
    D3DXMatrixScaling( &scale, radius, height, radius );
    D3DXMATRIX world;
    GetD3DMatrixFromBulletTransform( lit->mGlobalTransform, world );
    D3DXMatrixMultiply( &world, &scale, &world );
    bvRenderer->Bind();
    D3DXMATRIX wvp;
    D3DXMatrixMultiply( &wvp, &world, &Camera::msCurrentCamera->mViewProjection );
    bvRenderer->SetTransform( wvp );
    mBoundingCone->DrawSubset( 0 );
}

void DeferredRenderer::EndFirstPassAndDoSecondPass() {
    OnEnd();

    if( mHDRShader && Engine::Instance().IsHDREnabled() ) {
        mHDRShader->SetAsRenderTarget();
    } else {
        if( Engine::Instance().IsFXAAEnabled() ) {
            mFXAA->BeginDrawIntoTexture();
        } else {
            mGBuffer->BindBackSurfaceAsRT();
        }
    }

    Engine::Instance().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
	
	mGBuffer->BindTextures();
	Engine::Instance().GetDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
	mAmbientLightShader->Bind();
	Engine::Instance().SetPixelShaderMatrix( 1, &Camera::msCurrentCamera->invViewProjection );
	Engine::Instance().SetPixelShaderFloat3( 6, Camera::msCurrentCamera->GetPosition().elements );
	float fogParams[] = { 128, 256 - 128, 0 };
	Engine::Instance().SetPixelShaderFloat3( 7, fogParams );
	mFullscreenQuad->Bind();
	mFullscreenQuad->Render();

	if( Camera::msCurrentCamera ) {
		if( Camera::msCurrentCamera->mSkybox ) {
			if( Engine::Instance().IsHDREnabled() ) {
				Engine::Instance().GetDevice()->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
				Engine::Instance().GetDevice()->SetSamplerState( 2, D3DSAMP_SRGBTEXTURE, FALSE );
			}
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILENABLE, FALSE );
			mSkyboxShader->Bind( Camera::msCurrentCamera->mGlobalTransform.getOrigin() );
			Camera::msCurrentCamera->mSkybox->Render( );		
			if( Engine::Instance().IsHDREnabled() ) {
				Engine::Instance().GetDevice()->SetRenderState( D3DRS_SRGBWRITEENABLE, TRUE );
				Engine::Instance().GetDevice()->SetSamplerState( 2, D3DSAMP_SRGBTEXTURE, TRUE );
			}
		}  
		mGBuffer->BindTextures();
		mFullscreenQuad->Bind();
	}
	
	Engine::Instance().GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	// begin occlusion queries
	Engine::Instance().GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	Engine::Instance().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
	Engine::Instance().GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	

	int countInFrustum = 0;
	for( auto pLight : Light::msPointLightList ) {
		if( pLight->mQueryDone ) {
			if( Camera::msCurrentCamera->mFrustum.IsSphereInside( pLight->GetRealPosition(), pLight->GetRadius() ) && pLight->IsVisible() ) {
				auto iter = find( Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.begin(), Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.end(), pLight );
				if( iter == Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.end() ) {
					pLight->pQuery->Issue( D3DISSUE_BEGIN );
					RenderStar( pLight );
					pLight->pQuery->Issue( D3DISSUE_END );
				}
				pLight->inFrustum = true;
				countInFrustum++;
				pLight->mQueryDone = false;
			} else {
				pLight->inFrustum = false;
			}
		}
	}

	Engine::Instance().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
	Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILENABLE, TRUE );
	Engine::Instance().GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	
	for( auto pLight : Light::msPointLightList ) {
		if( pLight->IsVisible() ) {
			auto iter = find( Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.begin(), Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.end(), pLight );			
			DWORD pixelsVisible = 0;
			if( pLight->inFrustum  && !pLight->mQueryDone ) {
				if( iter == Camera::msCurrentCamera->mNearestPathPoint->mVisibleLightList.end() ) {
					HRESULT result = pLight->pQuery->GetData( &pixelsVisible, sizeof( pixelsVisible ), D3DGETDATA_FLUSH ) ;
					if( result == S_OK ) {
						pLight->mQueryDone = true;
						if( pixelsVisible > 0 ) {				
							pLight->trulyVisible = true;
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
		if( pLight->inFrustum  ) {
			if( pLight->pointTexture ) {
				mPointLightShader->pixelShaderTexProj->Bind();
			} else {
				mPointLightShader->pixelShader->Bind();
			}

			Engine::Instance().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );			

			Engine::Instance().GetDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );

			RenderSphere( pLight );

			mPointLightShader->SetLight( Camera::msCurrentCamera->invViewProjection, pLight );

			mFullscreenQuad->BindNoShader();

			Engine::Instance().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );

			mFullscreenQuad->Render();			
		}		
    }
	
    // Render spot lights
    for( auto pLight : Light::msSpotLightList ) {
		if( Camera::msCurrentCamera->mFrustum.IsSphereInside( pLight->GetRealPosition(), pLight->GetRadius() ) && pLight->IsVisible()  ) {
			if( Engine::Instance().IsSpotLightShadowsEnabled() ) {
				IDirect3DSurface9 * prevSurface = nullptr;
				if( mHDRShader && Engine::Instance().IsHDREnabled() ) {
					prevSurface = mHDRShader->hdrSurface;
				} else if( Engine::Instance().IsFXAAEnabled() ) {
					prevSurface = mFXAA->renderTarget;
				} else {
					prevSurface = mGBuffer->backSurface;
				}
				mSpotLightShadowMap->UnbindSpotShadowMap( 4 );

				Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILENABLE, FALSE );
				Engine::Instance().GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				Engine::Instance().GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

				mSpotLightShadowMap->RenderSpotShadowMap( prevSurface, 0, pLight );

				Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILENABLE, TRUE );
				Engine::Instance().GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				Engine::Instance().GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

				mSpotLightShadowMap->BindSpotShadowMap( 4 );
			}

			Engine::Instance().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

			RenderConeIntoStencilBuffer( pLight );

			mSpotLightShader->SetLight( Camera::msCurrentCamera->invViewProjection, pLight );

			Engine::Instance().GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
			Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );


			mFullscreenQuad->Bind();
			mFullscreenQuad->Render();
		}
    }
	
    if( mHDRShader && Engine::Instance().IsHDREnabled() ) {
		Engine::Instance().GetDevice()->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
		Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILENABLE, FALSE );
		Engine::Instance().GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );
		Engine::Instance().GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		Engine::Instance().SetDiffuseNormalSamplersFiltration( D3DTEXF_POINT, true );

        mHDRShader->CalculateFrameLuminance( );
        if( Engine::Instance().IsFXAAEnabled() ) {
            mHDRShader->DoToneMapping( mFXAA->renderTarget );
            mFXAA->DoAntialiasing( mFXAA->texture );
        } else {
            mHDRShader->DoToneMapping( mGBuffer->backSurface );
        }
    } else {
        if( Engine::Instance().IsFXAAEnabled() ) {
            Engine::Instance().GetDevice()->SetRenderState( D3DRS_STENCILENABLE, FALSE );
            Engine::Instance().GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );

			Engine::Instance().SetDiffuseNormalSamplersFiltration( D3DTEXF_POINT, true );

            mFXAA->DoAntialiasing( mFXAA->texture );
        }
    }
	
	if( Engine::Instance().IsAnisotropicFilteringEnabled() ) {
		Engine::Instance().SetDiffuseNormalSamplersFiltration( D3DTEXF_ANISOTROPIC, false );
	} else {
		Engine::Instance().SetDiffuseNormalSamplersFiltration( D3DTEXF_LINEAR, false );
	}

	Engine::Instance().GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
}


void DeferredRenderer::SetSpotLightShadowMapSize( int size ) {
    if( size != mSpotLightShadowMap->iSize ) {
        if( mSpotLightShadowMap ) {
            mSpotLightShadowMap.reset();
        }
        mSpotLightShadowMap = make_shared<SpotlightShadowMap>( size );
    }
}

void DeferredRenderer::OnLostDevice() {
	mBoundingStar->Release();
	mBoundingSphere->Release();
	mBoundingCone->Release();
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

	Engine::Instance().SetVertexShaderMatrix( 0, &matrix );
}

DeferredRenderer::SkyboxShader::~SkyboxShader() {
	delete mPixelShader;
	delete mVertexShader;
}

DeferredRenderer::SkyboxShader::SkyboxShader() {
	mVertexShader = new VertexShader( "data/shaders/skybox.vso" );
	mPixelShader = new PixelShader( "data/shaders/skybox.pso" );
}
