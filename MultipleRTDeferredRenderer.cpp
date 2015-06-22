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
    float constantRegister[4];
    D3DXMATRIX world, vwp, worldView;
	for( auto pOwner : mesh->GetOwners() ) {
		bool visible = pOwner->mSkinned || pOwner->IsVisible();
		pOwner->mInFrustum = Camera::msCurrentCamera->mFrustum.IsAABBInside( mesh->mAABB, ruVector3( pOwner->mGlobalTransform.getOrigin().m_floats ));
		if( visible ) {
			if( fabs( pOwner->mDepthHack ) > 0.001 ) {
				Camera::msCurrentCamera->EnterDepthHack( fabs( pOwner->mDepthHack ) );
			}
			D3DXMatrixIdentity( &world );
			if( !pOwner->mSkinned ) {
				GetD3DMatrixFromBulletTransform( pOwner->mGlobalTransform, world );
			}
			mesh->BindBuffers();
			D3DXMatrixMultiply( &vwp, &world, &Camera::msCurrentCamera->mViewProjection );
			D3DXMatrixMultiply( &worldView, &world, &Camera::msCurrentCamera->mView );
			// pass albedo
			constantRegister[0] = pOwner->mAlbedo;
			Engine::Instance().GetDevice()->SetPixelShaderConstantF( 0, constantRegister, 1 );
			// pass far z plane
			constantRegister[0] = Camera::msCurrentCamera->mFarZ;
			Engine::Instance().GetDevice()->SetPixelShaderConstantF( 1, constantRegister, 1 );
			// pass vertex shader matrices
			Engine::Instance().GetDevice()->SetVertexShaderConstantF( 0, &world.m[0][0], 4 );
			Engine::Instance().GetDevice()->SetVertexShaderConstantF( 5, &vwp.m[0][0], 4 );
			Engine::Instance().GetDevice()->SetVertexShaderConstantF( 10, &worldView.m[0][0], 4 );
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
	mGBufferVertexShader->Bind();
	mGBufferPixelShader->Bind();
}

MultipleRTDeferredRenderer::MultipleRTDeferredRenderer() {
	mGBufferVertexShader = new VertexShader( "data/shaders/deferredGBufferHQ.vso" );
	mGBufferPixelShader = new PixelShader( "data/shaders/deferredGBufferHQ.pso" );
}

MultipleRTDeferredRenderer::~MultipleRTDeferredRenderer() {
    delete mGBufferVertexShader;
    delete mGBufferPixelShader;
}
