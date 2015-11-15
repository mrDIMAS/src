#include "Precompiled.h"
#include "Engine.h"
#include "MultipleRTDeferredRenderer.h"
#include "Mesh.h"
#include "SceneNode.h"
#include "Utility.h"
#include "Camera.h"

void MultipleRTDeferredRenderer::OnEnd() {
    mGBuffer->UnbindRenderTargets();
}

void MultipleRTDeferredRenderer::RenderMesh( Mesh * mesh ) {
    D3DXMATRIX world, vwp;
	for( auto pOwner : mesh->GetOwners() ) {
		bool visible = true;
		if( pOwner->mIsBone ) {
			visible = false;
		} else {
			visible = pOwner->IsVisible();
		}
		pOwner->mInFrustum |= Camera::msCurrentCamera->mFrustum.IsAABBInside( mesh->mAABB, ruVector3( pOwner->mGlobalTransform.getOrigin().m_floats ));
		if( visible && ( pOwner->mInFrustum || pOwner->mIsSkinned ) ) {
			if( fabs( pOwner->mDepthHack ) > 0.001 ) {
				Camera::msCurrentCamera->EnterDepthHack( fabs( pOwner->mDepthHack ) );
			}
			D3DXMatrixIdentity( &world );
			if( !pOwner->mIsSkinned ) {
				GetD3DMatrixFromBulletTransform( pOwner->mGlobalTransform, world );
			}
			D3DXMatrixMultiply( &vwp, &world, &Camera::msCurrentCamera->mViewProjection );
			// pass albedo
			Engine::Instance().SetPixelShaderFloat( 0, pOwner->mAlbedo );
			// pass far z plane
			Engine::Instance().SetPixelShaderFloat( 1, Camera::msCurrentCamera->mFarZ );
			// pass vertex shader matrices
			Engine::Instance().SetVertexShaderMatrix( 0, &world );
			Engine::Instance().SetVertexShaderMatrix( 5, &vwp );
			if( mUsePOM ) {
				Engine::Instance().SetVertexShaderVector3( 10, Camera::msCurrentCamera->GetPosition() );
			}
			mesh->Render();
			if( pOwner->mDepthHack ) {
				Camera::msCurrentCamera->LeaveDepthHack();
			}
		}
	}
}

void MultipleRTDeferredRenderer::BeginFirstPass() {
    mGBuffer->BindRenderTargets();
    Engine::Instance().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
	//mCurrentVertexShader->Bind();
	//mCurrentPixelShader->Bind();
}

MultipleRTDeferredRenderer::MultipleRTDeferredRenderer( bool usePOM ) {
	// Parallax occlusion mapping shaders
	mVertexShaderPOM = new VertexShader( "data/shaders/deferredGBufferPOM.vso" );
	mPixelShaderPOM = new PixelShader( "data/shaders/deferredGBufferPOM.pso" );
	// Standard GBuffer shader
	mVertexShader = new VertexShader( "data/shaders/deferredGBuffer.vso" );
	mPixelShader = new PixelShader( "data/shaders/deferredGBuffer.pso" );
	// select proper shader
	SetPOMEnabled( usePOM );
}

MultipleRTDeferredRenderer::~MultipleRTDeferredRenderer() {
    delete mVertexShader;
    delete mPixelShader;
	delete mVertexShaderPOM;
	delete mPixelShaderPOM;
}

void MultipleRTDeferredRenderer::SetPOMEnabled( bool state ) {
	if( mUsePOM != state ) {
		mUsePOM = state;
		if( mUsePOM ) {
			mCurrentPixelShader = mPixelShaderPOM;
			mCurrentVertexShader = mVertexShaderPOM;
		} else {
			mCurrentPixelShader = mPixelShader;
			mCurrentVertexShader = mVertexShader;
		}
	}
}
