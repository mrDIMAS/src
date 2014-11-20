#include "ForwardRenderer.h"

ForwardRenderer * g_forwardRenderer = nullptr;

void ForwardRenderer::RenderMeshes() {
    IDirect3DStateBlock9 * state = nullptr;
    CheckDXError( g_device->CreateStateBlock( D3DSBT_ALL, &state ));

    pixelShader->Bind();
    vertexShader->Bind();

    CheckDXError( g_device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE ));
    CheckDXError( g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ));
    CheckDXError( g_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE ));
    CheckDXError( g_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE ));
    CheckDXError( g_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ));
    CheckDXError( g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW ));
    CheckDXError( g_device->SetRenderState( D3DRS_STENCILENABLE, FALSE ));

    for( auto group : renderList ) {
        IDirect3DTexture9 * diffuseTexture = group.first;
        vector< Mesh* > & meshes = group.second;

        g_device->SetTexture( 0, diffuseTexture );

        for( auto mesh : meshes ) {
            D3DXMATRIX world, wvp;
            GetD3DMatrixFromBulletTransform( mesh->ownerNode->globalTransform, world );
            D3DXMatrixMultiplyTranspose( &wvp, &world, &g_camera->viewProjection );

            CheckDXError( g_device->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 ));
            CheckDXError( g_device->SetPixelShaderConstantF( 0, &mesh->opacity, 1 ));

            mesh->BindBuffers();
            mesh->Render();
        }
    }

    CheckDXError( state->Apply());
    state->Release();
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
        "   return float4( float3( diffuseTexel.xyz * opacity), 1.0f );\n"
        "};\n";

    pixelShader = new PixelShader( pixelShaderSource );
}
