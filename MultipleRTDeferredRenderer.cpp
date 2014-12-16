#include "MultipleRTDeferredRenderer.h"
#include "Mesh.h"
#include "SceneNode.h"
#include "Utility.h"
#include "Camera.h"

void MultipleRTDeferredRenderer::OnEnd() {
    gBuffer->UnbindRenderTargets();
}

void MultipleRTDeferredRenderer::RenderMesh( Mesh * mesh ) {
    float constantRegister[4];
    D3DXMATRIX world, vwp;
    if( fabs( mesh->ownerNode->fDepthHack ) > 0.001 ) {
        g_camera->EnterDepthHack( fabs( mesh->ownerNode->fDepthHack ) );
    }
    D3DXMatrixIdentity( &world );
    if( !mesh->GetParentNode()->skinned ) {
        GetD3DMatrixFromBulletTransform( mesh->GetParentNode()->globalTransform, world );
    }
    mesh->BindBuffers();
    D3DXMatrixMultiply( &vwp, &world, &g_camera->viewProjection );
    constantRegister[0] = mesh->GetParentNode()->albedo;
    g_pDevice->SetPixelShaderConstantF( 0, constantRegister, 1 );
    g_pDevice->SetVertexShaderConstantF( 0, &world.m[0][0], 4 );
    g_pDevice->SetVertexShaderConstantF( 5, &vwp.m[0][0], 4 );
    mesh->Render();
    if( mesh->ownerNode->fDepthHack ) {
        g_camera->LeaveDepthHack();
    }
}

void MultipleRTDeferredRenderer::BeginFirstPass() {
    gBuffer->BindRenderTargets();
    CheckDXErrorFatal( g_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 ));
    if( renderQuality > 0 ) {
        vsGBufferHighQuality->Bind();
        psGBufferHighQuality->Bind();
    } else {
        vsGBufferLowQuality->Bind();
        psGBufferLowQuality->Bind();
    }
}

MultipleRTDeferredRenderer::MultipleRTDeferredRenderer() {
    CreateHighQualityShaders();
    CreateLowQualityShaders();
}

void MultipleRTDeferredRenderer::CreateHighQualityShaders() {
    vsGBufferHighQuality = new VertexShader( "data/shaders/deferredGBufferHQ.vso", true );
    psGBufferHighQuality = new PixelShader( "data/shaders/deferredGBufferHQ.pso", true );
}

void MultipleRTDeferredRenderer::CreateLowQualityShaders() {
    vsGBufferLowQuality = new VertexShader( "data/shaders/deferredGBufferLQ.vso", true );
    psGBufferLowQuality = new PixelShader( "data/shaders/deferredGBufferLQ.pso", true );
}

MultipleRTDeferredRenderer::~MultipleRTDeferredRenderer() {
    delete vsGBufferHighQuality;
    delete psGBufferHighQuality;
    delete vsGBufferLowQuality;
    delete psGBufferLowQuality;
}
