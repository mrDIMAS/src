#include "MultipleRTDeferredRenderer.h"
#include "Mesh.h"
#include "SceneNode.h"
#include "Utility.h"
#include "Camera.h"

void MultipleRTDeferredRenderer::OnEnd() {
    gBuffer->UnbindRenderTargets();
}

void MultipleRTDeferredRenderer::RenderMesh( Mesh * mesh ) {
    if( fabs( mesh->parent->fDepthHack ) > 0.001 )
        g_camera->EnterDepthHack( fabs( mesh->parent->fDepthHack ) );

    D3DXMATRIX world;
    D3DXMatrixIdentity( &world );

    if( mesh->GetParentNode()->skinned == false ) {
        GetD3DMatrixFromBulletTransform( mesh->GetParentNode()->globalTransform, world );
    }

    mesh->BindBuffers();

    D3DXMATRIX vwp;
    D3DXMatrixMultiply( &vwp, &world, &g_camera->viewProjection );

    pixelShaderPassOne->Bind();
    pixelShaderPassOne->GetConstantTable()->SetFloat( g_device, pAlbedo, mesh->GetParentNode()->albedo );

    vertexShaderPassOne->GetConstantTable()->SetMatrix( g_device, v1WVP, &vwp );
    vertexShaderPassOne->GetConstantTable()->SetMatrix( g_device, v1World, &world );

    mesh->Render();

    if( mesh->parent->fDepthHack )
        g_camera->LeaveDepthHack();
}

void MultipleRTDeferredRenderer::BeginFirstPass() {
    DWORD clearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    gBuffer->BindRenderTargets();

    g_device->Clear( 0, 0, clearFlags, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
    vertexShaderPassOne->Bind();

    g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
}

MultipleRTDeferredRenderer::MultipleRTDeferredRenderer() {
    InitPassOneShaders();
}

void MultipleRTDeferredRenderer::InitPassOneShaders() {
    // PASS 1 - Filling the G-Buffer
    string vertexSourcePassOne =
        "float4x4 g_world;\n"
        "float4x4 g_WVP;\n"

        "struct VS_INPUT {\n"
        "  float4 position : POSITION;\n"
        "  float3 normal   : NORMAL;\n"
        "  float2 texCoord : TEXCOORD0;\n"
        "  float3 tangent : TANGENT;\n"
        "};\n"

        "struct VS_OUTPUT\n"
        "{\n"
        "  float4 position : POSITION;\n"
        "  float4 screenPosition : TEXCOORD0;\n"
        "  float3 normal : TEXCOORD1;\n"
        "  float2 diffuseMapCoord : TEXCOORD2;\n"
        "  float3 binormal : TEXCOORD3;\n"
        "  float3 tangent : TEXCOORD4;\n"
        "};\n"

        "VS_OUTPUT main(VS_INPUT input)\n"
        "{\n"
        "  VS_OUTPUT output;\n"
        // compute screen vertex position
        "  output.position = mul(input.position, g_WVP );\n"
        // compute normal in world space
        "  output.normal = normalize( mul(input.normal, (float3x3)g_world));\n"
        // compute tangent in world space
        "  output.tangent = normalize( mul(input.tangent, (float3x3)g_world));\n"
        // compute binormal as a cross product of out normal and tangent
        "  output.binormal = normalize( cross( output.tangent, output.normal ));\n"
        // pass vertex screen position into pixel shader
        "  output.screenPosition = output.position;\n"
        // write texcoord
        "  output.diffuseMapCoord = input.texCoord;\n"
        "  return output;\n"
        "};\n";

    vertexShaderPassOne = new VertexShader( vertexSourcePassOne );

    v1World = vertexShaderPassOne->GetConstantTable()->GetConstantByName( 0, "g_world" );
    v1WVP = vertexShaderPassOne->GetConstantTable()->GetConstantByName( 0, "g_WVP" );

    string pixelSourcePassOne =
        "sampler diffuseSampler : register(s0);\n"
        "sampler normalSampler : register(s1);\n"

        "struct PS_INPUT {\n"
        "  float4 screenPos : TEXCOORD0;\n"
        "  float3 normal : TEXCOORD1;\n"
        "  float2 diffuseMapCoord : TEXCOORD2;\n"
        "  float3 binormal : TEXCOORD3;\n"
        "  float3 tangent : TEXCOORD4;\n"
        "};\n"

        "struct MRT_OUTPUT {\n"
        "  float4 depth : COLOR0;\n"
        "  float4 normal : COLOR1;\n"
        "  float4 diffuseMap : COLOR2;\n"
        "};\n"

#ifndef USE_R32F_DEPTH
        "float4 packFloatToVec4i ( const float value )\n"
        "{\n"
        "  const float4 bitSh   = float4 ( 256.0*256.0*256.0, 256.0 * 256.0, 256.0, 1.0 );\n"
        "  const float4 bitMask = float4 ( 0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0 )\n;"

        "  float4	res = frac ( value * bitSh );\n"

        "  return res - res.xxyz * bitMask;\n"
        "}\n"
#endif

        "float albedo = 0.0f;"

        "MRT_OUTPUT main( PS_INPUT input )\n"
        "{\n"
        "   MRT_OUTPUT output = (MRT_OUTPUT)0;\n"

        // get diffuse color
        "   output.diffuseMap = tex2D( diffuseSampler, input.diffuseMapCoord );\n"

        // do alpha test
        "   if( output.diffuseMap.a < 0.1 )\n"
        "     discard;\n"

        "   output.diffuseMap.a = albedo;\n"

        // get normal from normal map
        "   float4 normal = normalize( tex2D( normalSampler, input.diffuseMapCoord ) * 2.0 - 1.0f );\n"

        // construct tangent space matrix
        "   float3x3 tangentSpace;\n"
        "   tangentSpace[0] = input.tangent;\n"
        "   tangentSpace[1] = input.binormal;\n"
        "   tangentSpace[2] = input.normal;\n"

        // transform normal from normal map to tangent space
        "   normal = float4( mul( normal.xyz, tangentSpace ), 1.0f );\n"

        // pack normal to [ 0; 1 ]
        "   normal = float4( normal.xyz * 0.5f + 0.5f, 1.0f );"
#ifdef USE_R32F_DEPTH
        "   output.depth = float4( input.screenPos.z / input.screenPos.w, 0, 0, 1.0 );\n"
#else
        "   output.depth = packFloatToVec4i( input.screenPos.z / input.screenPos.w );\n"
#endif
        "   output.normal = normal;\n"
        "   return output;"
        "};\n"
        ;

    pixelShaderPassOne = new PixelShader( pixelSourcePassOne );

    pAlbedo = pixelShaderPassOne->GetConstantTable()->GetConstantByName( 0, "albedo" );
}

MultipleRTDeferredRenderer::~MultipleRTDeferredRenderer() {
    if( vertexShaderPassOne ) {
        delete vertexShaderPassOne;
    }
    if( pixelShaderPassOne ) {
        delete pixelShaderPassOne;
    }
}
