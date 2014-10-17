#include "LightmapRenderer.h"
#include "Camera.h"
#include "Utility.h"

LightmapRenderer * g_lightMapRenderer = 0;

void LightmapRenderer::End() {
    g_device->EndScene();
}

void LightmapRenderer::RenderMesh( Mesh * mesh ) {
    mesh->BindBuffers();

    D3DXMATRIX world;
    GetD3DMatrixFromBulletTransform( mesh->GetParentNode()->GetGlobalTransform(), world );

    D3DXMATRIX vwp;
    D3DXMatrixIdentity( &vwp );
    D3DXMatrixMultiply( &vwp, &world, &g_camera->view );
    D3DXMatrixMultiply( &vwp, &vwp, &g_camera->projection );

    vertexShader->GetConstantTable()->SetMatrix( g_device, vWVP, &vwp );

    mesh->Render();
}

void LightmapRenderer::Begin() {
    DWORD clearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    g_device->Clear( 0, 0, clearFlags, D3DCOLOR_XRGB( 100, 0, 0 ), 1.0, 0 );

    g_device->BeginScene( );

    vertexShader->Bind();
    pixelShader->Bind();

    g_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
    g_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE );
    g_device->SetRenderState( D3DRS_LIGHTING, FALSE );
}

LightmapRenderer::LightmapRenderer() {
    string vertexShaderSource =
        "float4x4 wvp;\n"
        //"float4x4 g_view;\n"
        //"float4x4 g_projection;\n"

        "struct VS_INPUT\n"
        "{\n"
        "   float4 position : POSITION;\n"
        "   float2 texCoords : TEXCOORD0;\n"
        "   float2 secondTexCoords : TEXCOORD1;\n"
        "};\n"

        "struct VS_OUTPUT\n"
        "{\n"
        "   float4 position : POSITION;\n"
        "   float2 texCoords : TEXCOORD0;\n"
        "   float2 secondTexCoords : TEXCOORD1;\n"
        "};\n"

        "VS_OUTPUT main( VS_INPUT input )\n"
        "{\n"
        "   VS_OUTPUT output;\n"

        "   output.position = mul( input.position, wvp );\n"
        "   output.texCoords = input.texCoords;\n"
        "   output.secondTexCoords = input.secondTexCoords;\n"

        "   return output;"
        "}\n";

    vertexShader = new VertexShader( vertexShaderSource );

    vWVP = vertexShader->GetConstantTable()->GetConstantByName( 0, "wvp" );

    string pixelShaderSource =
        "texture diffuseMap;\n"

        "sampler diffuseSampler : register(s0) = sampler_state\n"
        "{\n"
        "   texture = <diffuseMap>;\n"
        "};\n"

        "texture lightMap;\n"

        "sampler lightSampler : register(s1) = sampler_state\n"
        "{\n"
        "   texture = <lightMap>;\n"
        "};\n"

        "struct PS_INPUT\n"
        "{\n"
        "   float2 texCoords : TEXCOORD0;\n"
        "   float2 secondTexCoords : TEXCOORD1;\n"
        "};\n"

        "float4 main( PS_INPUT input ) : COLOR0\n"
        "{\n"
        "   float4 diffuseTexel = tex2D( diffuseSampler, input.texCoords );\n"
        "   float4 lightTexel = tex2D( lightSampler, input.secondTexCoords );\n"
        "   return diffuseTexel * lightTexel;"
        "};\n";

    pixelShader = new PixelShader( pixelShaderSource );
}
