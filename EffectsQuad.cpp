#include "EffectsQuad.h"

void EffectsQuad::Render()
{
    CheckDXErrorFatal( g_pDevice->SetStreamSource( 0, vertexBuffer, 0, sizeof( QuadVertex )));
    CheckDXErrorFatal( g_pDevice->SetVertexDeclaration( vertexDeclaration ));
    CheckDXErrorFatal( g_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 ));
    if( debug )
    {
        CheckDXErrorFatal( g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ));
        CheckDXErrorFatal( g_pDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE ));
    }
}

void EffectsQuad::Bind()
{
    if( debug )
    {
        debugPixelShader->Bind();
        CheckDXErrorFatal( g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ));
        CheckDXErrorFatal( g_pDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE ));
    }
    vertexShader->Bind();
    CheckDXErrorFatal( vertexShader->GetConstantTable()->SetMatrix( g_pDevice, v2Proj, &orthoProjection ));
}

EffectsQuad::~EffectsQuad()
{
    if( vertexDeclaration )
        vertexDeclaration->Release();

    if( vertexBuffer )
        vertexBuffer->Release();

    if( debugPixelShader )
        delete debugPixelShader;

    delete vertexShader;
}

void EffectsQuad::SetSize( float width, float height )
{
    if( !debug )
    {
        QuadVertex vertices[ ] =
        {
            {        - 0.5, -0.5, 0, 0, 0 }, { width - 0.5,         - 0.5, 0, 1, 0 }, { -0.5, height - 0.5, 0, 0, 1 },
            {  width - 0.5, -0.5, 0, 1, 0 }, { width - 0.5,  height - 0.5, 0, 1, 1 }, { -0.5, height - 0.5, 0, 0, 1 }
        };

        void * data = 0;

        CheckDXErrorFatal( vertexBuffer->Lock( 0, 0, &data, 0 ));
        memcpy( data, vertices, sizeof( QuadVertex ) * 6 );
        CheckDXErrorFatal( vertexBuffer->Unlock( ));
    }
}

EffectsQuad::EffectsQuad( bool bDebug )
{
    CheckDXErrorFatal( g_pDevice->CreateVertexBuffer( 6 * sizeof( QuadVertex ), D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vertexBuffer, 0 ));

    debug = bDebug;
    debugPixelShader = nullptr;

    if( !debug )
    {
        QuadVertex vertices[ ] =
        {
            {           -0.5, -0.5, 0, 0, 0 }, { g_width - 0.5,           - 0.5, 0, 1, 0 }, { -0.5, g_height - 0.5, 0, 0, 1 },
            {  g_width - 0.5, -0.5, 0, 1, 0 }, { g_width - 0.5,  g_height - 0.5, 0, 1, 1 }, { -0.5, g_height - 0.5, 0, 0, 1 }
        };

        void * data = 0;

        CheckDXErrorFatal( vertexBuffer->Lock( 0, 0, &data, 0 ));
        memcpy( data, vertices, sizeof( QuadVertex ) * 6 );
        CheckDXErrorFatal( vertexBuffer->Unlock( ));
    }
    else
    {
        int size = 500;
        QuadVertex vertices[ ] =
        {
            {        -0.5, -0.5, 0, 0, 0 }, { size - 0.5,        -0.5, 0, 1, 0 }, { -0.5, size - 0.5, 0, 0, 1 },
            {  size - 0.5, -0.5, 0, 1, 0 }, { size - 0.5,  size - 0.5, 0, 1, 1 }, { -0.5, size - 0.5, 0, 0, 1 }
        };

        void * data = 0;

        CheckDXErrorFatal( vertexBuffer->Lock( 0, 0, &data, 0 ));
        memcpy( data, vertices, sizeof( QuadVertex ) * 6 );
        CheckDXErrorFatal( vertexBuffer->Unlock( ));

        string debugPixelShaderSource =
            "sampler diffuse : register( s4 );\n"
            "float4 main( float2 texCoord : TEXCOORD0 ) : COLOR0 {\n"
            "   return tex2D( diffuse, texCoord );\n"
            "};\n";

        debugPixelShader = new PixelShader( debugPixelShaderSource );
    }

    D3DVERTEXELEMENT9 quadVertexDeclation[ ] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    CheckDXErrorFatal( g_pDevice->CreateVertexDeclaration( quadVertexDeclation, &vertexDeclaration ));

    string vertexSourcePassTwo =
        "float4x4 g_projection;\n"

        "struct VS_INPUT {\n"
        "  float4 position : POSITION;\n"
        "  float3 texcoord : TEXCOORD0;\n"
        "};\n"

        "struct VS_OUTPUT {\n"
        "  float4 position : POSITION;\n"
        "  float3 texcoord : TEXCOORD0;\n"
        "};\n"

        "VS_OUTPUT main(VS_INPUT input) {\n"
        "  VS_OUTPUT output;\n"
        "  output.position   = mul(input.position, g_projection);\n"
        "  output.texcoord   = input.texcoord;\n"
        "  return output;\n"
        "};\n";

    vertexShader = new VertexShader( vertexSourcePassTwo );

    v2Proj = vertexShader->GetConstantTable()->GetConstantByName( 0, "g_projection" );

    D3DXMatrixOrthoOffCenterLH ( &orthoProjection, 0, g_width, g_height, 0, 0, 1024 );
}
