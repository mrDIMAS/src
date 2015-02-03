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
    if( fabs( mesh->mOwnerNode->mDepthHack ) > 0.001 ) {
        g_camera->EnterDepthHack( fabs( mesh->mOwnerNode->mDepthHack ) );
    }
    D3DXMatrixIdentity( &world );
    if( !mesh->GetOwner()->mSkinned ) {
        GetD3DMatrixFromBulletTransform( mesh->GetOwner()->mGlobalTransform, world );
    }
    mesh->BindBuffers();
    D3DXMatrixMultiply( &vwp, &world, &g_camera->viewProjection );
	D3DXMatrixMultiply( &worldView, &world, &g_camera->view );
	// pass albedo
    constantRegister[0] = mesh->GetOwner()->mAlbedo;
    gpDevice->SetPixelShaderConstantF( 0, constantRegister, 1 );
	// pass far z plane
	constantRegister[0] = g_camera->farZ;
	gpDevice->SetPixelShaderConstantF( 1, constantRegister, 1 );
	// pass vertex shader matrices
    gpDevice->SetVertexShaderConstantF( 0, &world.m[0][0], 4 );
    gpDevice->SetVertexShaderConstantF( 5, &vwp.m[0][0], 4 );
	gpDevice->SetVertexShaderConstantF( 10, &worldView.m[0][0], 4 );
    mesh->Render();
    if( mesh->mOwnerNode->mDepthHack ) {
        g_camera->LeaveDepthHack();
    }
}

void MultipleRTDeferredRenderer::BeginFirstPass() {
    mGBuffer->BindRenderTargets();
    CheckDXErrorFatal( gpDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 ));
	mGBufferVertexShader->Bind();
	mGBufferPixelShader->Bind();
}

MultipleRTDeferredRenderer::MultipleRTDeferredRenderer() {
	mGBufferVertexShader = new VertexShader( "data/shaders/deferredGBufferHQ.vso", true );
	mGBufferPixelShader = new PixelShader( "data/shaders/deferredGBufferHQ.pso", true );
}

MultipleRTDeferredRenderer::~MultipleRTDeferredRenderer() {
    delete mGBufferVertexShader;
    delete mGBufferPixelShader;
}
