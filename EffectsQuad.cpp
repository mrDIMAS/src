#include "EffectsQuad.h"

void EffectsQuad::Render() {
    g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( QuadVertex ));
    g_device->SetVertexDeclaration( vertexDeclaration );
    g_device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
}

void EffectsQuad::Bind() {
    vertexShader->Bind();
    vertexShader->GetConstantTable()->SetMatrix( g_device, v2Proj, &orthoProjection );
}

EffectsQuad::~EffectsQuad() {
    if( vertexDeclaration ) {
        vertexDeclaration->Release();
    }

    if( vertexBuffer ) {
        vertexBuffer->Release();
    }

    delete vertexShader;
}

EffectsQuad::EffectsQuad() {
    g_device->CreateVertexBuffer( 6 * sizeof( QuadVertex ), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vertexBuffer, 0 );

    QuadVertex vertices[ ] = { QuadVertex( -0.5, -0.5, 0, 0, 0 ), QuadVertex( g_width - 0.5, 0 - 0.5, 0, 1, 0 ), QuadVertex ( 0 - 0.5, g_height - 0.5, 0, 0, 1 ),
                               QuadVertex( g_width - 0.5, 0 - 0.5, 0, 1, 0 ), QuadVertex( g_width - 0.5, g_height - 0.5, 0, 1, 1 ), QuadVertex ( 0 - 0.5, g_height - 0.5, 0, 0, 1 )
                             };

    void * data = 0;

    vertexBuffer->Lock( 0, 0, &data, 0 );
    memcpy( data, vertices, sizeof( QuadVertex ) * 6 );
    vertexBuffer->Unlock( );

    D3DVERTEXELEMENT9 quadVertexDeclation[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    g_device->CreateVertexDeclaration( quadVertexDeclation, &vertexDeclaration );

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

EffectsQuad::QuadVertex::QuadVertex( float ax, float ay, float az, float atx, float aty ) {
    x = ax;
    y = ay;
    z = az;

    tx = atx;
    ty = aty;
}
