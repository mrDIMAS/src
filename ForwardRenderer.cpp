#include "ForwardRenderer.h"

ForwardRenderer * g_forwardRenderer = nullptr;

void ForwardRenderer::RenderMeshes() {
    pixelShader->Bind();
    vertexShader->Bind();

    for( auto group : renderList ) {
        IDirect3DTexture9 * diffuseTexture = group.first;
        vector< Mesh* > & meshes = group.second;

        gpDevice->SetTexture( 0, diffuseTexture );

        for( auto mesh : meshes ) {
            D3DXMATRIX world, wvp;
            GetD3DMatrixFromBulletTransform( mesh->ownerNode->globalTransform, world );
            D3DXMatrixMultiplyTranspose( &wvp, &world, &g_camera->viewProjection );

            CheckDXErrorFatal( gpDevice->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 ));
            CheckDXErrorFatal( gpDevice->SetPixelShaderConstantF( 0, &mesh->opacity, 1 ));

            mesh->BindBuffers();
            mesh->Render();
        }
    }
}

void ForwardRenderer::RemoveMesh( Mesh * mesh ) {
    auto groupIter = renderList.find( mesh->diffuseTexture->GetInterface() );
    if( groupIter != renderList.end() ) {
        auto & group = groupIter->second;
        group.erase( find( group.begin(), group.end(), mesh ));
    }
}

void ForwardRenderer::AddMesh( Mesh * mesh ) {
    renderList[ mesh->diffuseTexture->GetInterface() ].push_back( mesh );
}

ForwardRenderer::~ForwardRenderer() {
    delete pixelShader;
    delete vertexShader;
}

ForwardRenderer::ForwardRenderer() {
    string vertexShaderSource =
        "float4x4 worldViewProjection : register( c0 );\n"

        "struct VSInput {\n"
        "   float4 position : POSITION;\n"
        "   float2 texCoord : TEXCOORD0;\n"
        "};\n"

        "struct VSOutput {\n"
        "   float4 position : POSITION;\n"
        "   float2 texCoord : TEXCOORD0;\n"
        "};\n"

        "VSOutput main( VSInput input ) {\n"
        "   VSOutput output = (VSOutput)0;\n"
        "   output.position = mul( input.position, worldViewProjection );\n"
        "   output.texCoord = input.texCoord;\n"
        "   return output;\n"
        "};\n";

    vertexShader = new VertexShader( vertexShaderSource );

    string pixelShaderSource =
        "float opacity = 1.0f;\n"

        "sampler diffuseSampler : register( s0 );\n"

        "struct PSInput {\n"
        "   float2 texCoord : TEXCOORD0;\n"
        "};\n"

        "float4 main( PSInput input ) : COLOR0 {\n"
        "   float4 diffuseTexel = tex2D( diffuseSampler, input.texCoord );\n"
        "   return float4( diffuseTexel.rgb, diffuseTexel.a * opacity );\n"
        "};\n";

    pixelShader = new PixelShader( pixelShaderSource );
}
