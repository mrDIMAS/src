#include "SingleRTDeferredRenderer.h"
#include "Mesh.h"
#include "SceneNode.h"
#include "Utility.h"
#include "Camera.h"

////////////////////////////////////////////////////////////
// Pass one shader for single RenderTarget per pass
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Pass one diffuse map filling
////////////////////////////////////////////////////////////
SingleRTDeferredRenderer::Pass1FillDiffuseMap::Pass1FillDiffuseMap() {
    // PASS 1 - Filling the G-Buffer
    string vertexSourcePassOne =
        "float4x4 worldViewProj;\n"

        "struct VS_INPUT {\n"
        "  float4 position : POSITION;\n"
        "  float2 texCoord : TEXCOORD0;\n"
        "};\n"

        "struct VS_OUTPUT\n"
        "{\n"
        "  float4 position : POSITION;\n"
        "  float2 diffuseMapCoord : TEXCOORD2;\n"
        "};\n"

        "VS_OUTPUT main(VS_INPUT input)\n"
        "{\n"
        "  VS_OUTPUT output;\n"
        // compute screen vertex position
        "  output.position = mul(input.position, worldViewProj);\n"
        // write texcoord
        "  output.diffuseMapCoord = input.texCoord;\n"
        "  return output;\n"
        "};\n";

    vs = new VertexShader( vertexSourcePassOne );

    vWVP = vs->GetConstantTable()->GetConstantByName( 0, "worldViewProj" );

    string pixelSourcePassOne =

        "texture diffuseMap;\n"

        "sampler diffuseSampler : register(s0) = sampler_state\n"
        "{\n"
        "   texture = <diffuseMap>;\n"
        "};\n"

        "float4 main( float2 diffuseMapCoord : TEXCOORD2 ) : COLOR0\n"
        "{\n"

        "   float4 diffuseColor = tex2D( diffuseSampler, diffuseMapCoord );\n"

        "   clip( diffuseColor.a - 0.1 );"

        "   return diffuseColor;"
        "};\n"
        ;

    ps = new PixelShader( pixelSourcePassOne );
}
//////////////////////////////////////////////////////////////////////////

void SingleRTDeferredRenderer::Pass1FillDiffuseMap::Bind( ) {
    vs->Bind();
    ps->Bind();
};

void SingleRTDeferredRenderer::Pass1FillDiffuseMap::SetTransform( D3DXMATRIX & wvp ) {
    vs->GetConstantTable()->SetMatrix( g_device, vWVP, &wvp );
};

SingleRTDeferredRenderer::Pass1FillDiffuseMap::~Pass1FillDiffuseMap() {
    delete ps;
    delete vs;
};

////////////////////////////////////////////////////////////
// Pass one normal map filling
////////////////////////////////////////////////////////////

SingleRTDeferredRenderer::Pass1FillNormalMap::Pass1FillNormalMap() {
    // PASS 1 - Filling the G-Buffer
    string vertexSourcePassOne =
        "float4x4 worldViewProj;\n"
        "float4x4 g_world;"

        "struct VS_INPUT {\n"
        "  float4 position : POSITION;\n"
        "  float3 normal   : NORMAL;\n"
        "  float2 texCoord : TEXCOORD0;\n"
        "  float3 tangent : TANGENT;\n"
        "};\n"

        "struct VS_OUTPUT\n"
        "{\n"
        "  float4 position : POSITION;\n"
        "  float3 normal : TEXCOORD1;\n"
        "  float2 diffuseMapCoord : TEXCOORD2;\n"
        "  float3 binormal : TEXCOORD3;\n"
        "  float3 tangent : TEXCOORD4;\n"
        "};\n"

        "VS_OUTPUT main(VS_INPUT input)\n"
        "{\n"
        "  VS_OUTPUT output;\n"

        // compute screen vertex position
        "  output.position = mul(input.position, worldViewProj);\n"
        // compute normal in world space
        "  output.normal = normalize( mul(input.normal, (float3x3)g_world));\n"
        // compute tangent in world space
        "  output.tangent = normalize( mul(input.tangent, (float3x3)g_world));\n"
        // compute binormal as a cross product of out normal and tangent
        "  output.binormal = normalize( cross( output.tangent, output.normal ));\n"
        // write texcoord
        "  output.diffuseMapCoord = input.texCoord;\n"
        "  return output;\n"
        "};\n";

    vs = new VertexShader( vertexSourcePassOne );

    vWVP = vs->GetConstantTable()->GetConstantByName( 0, "worldViewProj" );
    vWorld = vs->GetConstantTable()->GetConstantByName( 0, "g_world" );

    string pixelSourcePassOne =

        "texture normalMap;\n"

        "sampler normalSampler : register(s1) = sampler_state\n"
        "{\n"
        "   texture = <normalMap>;\n"
        "};\n"

        "struct PS_INPUT {\n"
        "  float3 normal : TEXCOORD1;\n"
        "  float2 diffuseMapCoord : TEXCOORD2;\n"
        "  float3 binormal : TEXCOORD3;\n"
        "  float3 tangent : TEXCOORD4;\n"
        "};\n"

        "float4 main( PS_INPUT input ) : COLOR0\n"
        "{\n"
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

        "   return normal;"
        "};\n"
        ;

    ps = new PixelShader( pixelSourcePassOne );
};

void SingleRTDeferredRenderer::Pass1FillNormalMap::Bind( ) {
    vs->Bind();
    ps->Bind();
};

void SingleRTDeferredRenderer::Pass1FillNormalMap::SetTransform( D3DXMATRIX & world, D3DXMATRIX & wvp ) {
    vs->GetConstantTable()->SetMatrix( g_device, vWVP, &wvp );
    vs->GetConstantTable()->SetMatrix( g_device, vWorld, &world );
};

SingleRTDeferredRenderer::Pass1FillNormalMap::~Pass1FillNormalMap() {
    delete ps;
    delete vs;
};

////////////////////////////////////////////////////////////
// Pass one depth map filling
////////////////////////////////////////////////////////////

SingleRTDeferredRenderer::Pass1FillDepthMap::Pass1FillDepthMap() {
    // PASS 1 - Filling the G-Buffer
    string vertexSourcePassOne =
        "float4x4 worldViewProj;\n"

        "struct VS_INPUT {\n"
        "  float4 position : POSITION;\n"
        "};\n"

        "struct VS_OUTPUT\n"
        "{\n"
        "  float4 position : POSITION;\n"
        "  float4 screenPosition : TEXCOORD0;\n"
        "};\n"

        "VS_OUTPUT main(VS_INPUT input)\n"
        "{\n"
        "  VS_OUTPUT output;\n"

        // compute screen vertex position
        "  output.position = mul(input.position, worldViewProj);\n"

        // pass vertex screen position into pixel shader
        "  output.screenPosition = output.position;\n"

        "  return output;\n"
        "};\n";

    vs = new VertexShader( vertexSourcePassOne );

    vWVP = vs->GetConstantTable()->GetConstantByName( 0, "worldViewProj" );

    string pixelSourcePassOne =
        "float4 packFloatToVec4i ( const float value )\n"
        "{\n"
        "  const float4 bitSh   = float4 ( 256.0*256.0*256.0, 256.0 * 256.0, 256.0, 1.0 );\n"
        "  const float4 bitMask = float4 ( 0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0 )\n;"

        "  float4	res = frac ( value * bitSh );\n"

        "  return res - res.xxyz * bitMask;\n"
        "}\n"

        "float4 main( float4 screenPos : TEXCOORD0 ) : COLOR0\n"
        "{\n"

        "   return packFloatToVec4i( screenPos.z / screenPos.w );\n"

        "};\n"
        ;

    ps = new PixelShader( pixelSourcePassOne );
};

void SingleRTDeferredRenderer::Pass1FillDepthMap::Bind( ) {
    vs->Bind();
    ps->Bind();
};

void SingleRTDeferredRenderer::Pass1FillDepthMap::SetTransform( D3DXMATRIX & wvp ) {
    vs->GetConstantTable()->SetMatrix( g_device, vWVP, &wvp );
};

SingleRTDeferredRenderer::Pass1FillDepthMap::~Pass1FillDepthMap() {
    delete ps;
    delete vs;
};

void SingleRTDeferredRenderer::OnEnd() {
    gBuffer->BindBackSurfaceAsRT();
}

void SingleRTDeferredRenderer::RenderMesh( Mesh * mesh ) {
    D3DXMATRIX world;
    D3DXMatrixIdentity( &world );

    if( mesh->GetParentNode()->skinned == false ) {
        GetD3DMatrixFromBulletTransform( mesh->GetParentNode()->globalTransform, world );
    }

    mesh->BindBuffers();

    D3DXMATRIX vwp;
    D3DXMatrixMultiply( &vwp, &world, &g_camera->viewProjection );

    pass1FillDiffuseMap->SetTransform( vwp );
    pass1FillNormalMap->SetTransform( world, vwp );
    pass1FillDepthMap->SetTransform( vwp );

    mesh->Render();
}

void SingleRTDeferredRenderer::BeginFirstPass() {
    DWORD clearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    gBuffer->UnbindTextures();

    gBuffer->BindDiffuseMapAsRT();
    g_device->Clear( 0, 0, clearFlags, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

    gBuffer->BindNormalMapAsRT();
    g_device->Clear( 0, 0, clearFlags, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

    gBuffer->BindDepthMapAsRT();
    g_device->Clear( 0, 0, clearFlags, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

    g_device->BeginScene( );

    g_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
    g_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE );
}

SingleRTDeferredRenderer::SingleRTDeferredRenderer() {
    pass1FillDiffuseMap = new Pass1FillDiffuseMap;
    pass1FillNormalMap = new Pass1FillNormalMap;
    pass1FillDepthMap = new Pass1FillDepthMap;
}

void SingleRTDeferredRenderer::SetDepthPass() {
    gBuffer->BindDepthMapAsRT();
    pass1FillDepthMap->Bind();
}

void SingleRTDeferredRenderer::SetNormalPass() {
    gBuffer->BindNormalMapAsRT();
    pass1FillNormalMap->Bind();
}

void SingleRTDeferredRenderer::SetDiffusePass() {
    gBuffer->BindDiffuseMapAsRT();
    pass1FillDiffuseMap->Bind( );
}

SingleRTDeferredRenderer::~SingleRTDeferredRenderer() {
    delete pass1FillDiffuseMap;
    delete pass1FillNormalMap;
    delete pass1FillDepthMap;
}
