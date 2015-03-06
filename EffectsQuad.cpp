#include "EffectsQuad.h"
#include "Renderer.h"
#include "Camera.h"

void EffectsQuad::Render() {
    gpDevice->SetStreamSource( 0, vertexBuffer, 0, sizeof( QuadVertex ));
    gpDevice->SetVertexDeclaration( vertexDeclaration );
    gpDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
    if( debug ) {
        gpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        gpDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
    }
}

void EffectsQuad::Bind( bool bindInternalVertexShader ) {
    if( debug ) {
        debugPixelShader->Bind();
        gpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        gpDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
    }
	if( bindInternalVertexShader ) {
		vertexShader->Bind();
		gpRenderer->SetVertexShaderMatrix( 0, &orthoProjection );
		gpRenderer->SetVertexShaderMatrix( 5, &g_camera->invViewProjection );
		ruVector3 camPos = ruVector3( g_camera->mView._41, g_camera->mView._42, g_camera->mView._43 );
		gpRenderer->SetVertexShaderFloat3( 10, camPos.elements );
	}
}

void EffectsQuad::BindNoShader() {
	gpRenderer->SetVertexShaderMatrix( 0, &orthoProjection );
	gpRenderer->SetVertexShaderMatrix( 5, &g_camera->invViewProjection );
	ruVector3 camPos = ruVector3( g_camera->mView._41, g_camera->mView._42, g_camera->mView._43 );
	gpRenderer->SetVertexShaderFloat3( 10, camPos.elements );
}

EffectsQuad::~EffectsQuad() {
    Free();
    if( debugPixelShader ) {
        delete debugPixelShader;
    }
    delete vertexShader;
}

void EffectsQuad::SetSize( float width, float height ) {
    if( !debug ) {
        QuadVertex vertices[ ] = {
            {        - 0.5, -0.5, 0, 0, 0 }, { width - 0.5,         - 0.5, 0, 1, 0 }, { -0.5, height - 0.5, 0, 0, 1 },
            {  width - 0.5, -0.5, 0, 1, 0 }, { width - 0.5,  height - 0.5, 0, 1, 1 }, { -0.5, height - 0.5, 0, 0, 1 }
        };

        void * data = 0;

        CheckDXErrorFatal( vertexBuffer->Lock( 0, 0, &data, 0 ));
        memcpy( data, vertices, sizeof( QuadVertex ) * 6 );
        CheckDXErrorFatal( vertexBuffer->Unlock( ));
    }
}

EffectsQuad::EffectsQuad( bool bDebug ) {
    CheckDXErrorFatal( gpDevice->CreateVertexBuffer( 6 * sizeof( QuadVertex ), D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vertexBuffer, 0 ));

    debug = bDebug;
    debugPixelShader = nullptr;

    if( !debug ) {
        QuadVertex vertices[ ] = {
            {           -0.5, -0.5, 0, 0, 0 }, { g_width - 0.5,           - 0.5, 0, 1, 0 }, { -0.5, g_height - 0.5, 0, 0, 1 },
            {  g_width - 0.5, -0.5, 0, 1, 0 }, { g_width - 0.5,  g_height - 0.5, 0, 1, 1 }, { -0.5, g_height - 0.5, 0, 0, 1 }
        };

        void * data = 0;

        CheckDXErrorFatal( vertexBuffer->Lock( 0, 0, &data, 0 ));
        memcpy( data, vertices, sizeof( QuadVertex ) * 6 );
        CheckDXErrorFatal( vertexBuffer->Unlock( ));
    } else {
        int size = 500;
        QuadVertex vertices[ ] = {
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

    D3DVERTEXELEMENT9 quadVertexDeclation[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    gpDevice->CreateVertexDeclaration( quadVertexDeclation, &vertexDeclaration );

    vertexShader = new VertexShader( "data/shaders/quad.vso", true );
    D3DXMatrixOrthoOffCenterLH ( &orthoProjection, 0, g_width, g_height, 0, 0, 1024 );
}

void EffectsQuad::OnDeviceLost()
{
	Free();
	Initialize();
}

void EffectsQuad::Free()
{
	vertexDeclaration->Release();
	vertexBuffer->Release();
}

void EffectsQuad::Initialize()
{

}
