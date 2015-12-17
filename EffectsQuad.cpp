/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#include "Precompiled.h"
#include "EffectsQuad.h"
#include "Engine.h"
#include "Camera.h"

void EffectsQuad::Render() {
    Engine::I().GetDevice()->SetStreamSource( 0, vertexBuffer, 0, sizeof( QuadVertex ));
    Engine::I().GetDevice()->SetVertexDeclaration( vertexDeclaration );
    Engine::I().GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
    if( debug ) {
        Engine::I().SetAlphaBlendEnabled( true );
        Engine::I().SetStencilEnabled( true );
    }
}

void EffectsQuad::Bind( bool bindInternalVertexShader ) {
    if( debug ) {
        debugPixelShader->Bind();
        Engine::I().SetAlphaBlendEnabled( false );
        Engine::I().SetStencilEnabled( false );
    }
	if( bindInternalVertexShader ) {
		vertexShader->Bind();
		Engine::I().SetVertexShaderMatrix( 0, &orthoProjection );
		Engine::I().SetVertexShaderMatrix( 5, &Camera::msCurrentCamera->invViewProjection );
		ruVector3 camPos = ruVector3( Camera::msCurrentCamera->mView._41, Camera::msCurrentCamera->mView._42, Camera::msCurrentCamera->mView._43 );
		Engine::I().SetVertexShaderFloat3( 10, camPos.elements );
	}
}

void EffectsQuad::BindNoShader() {
	Engine::I().SetVertexShaderMatrix( 0, &orthoProjection );
	Engine::I().SetVertexShaderMatrix( 5, &Camera::msCurrentCamera->invViewProjection );
	ruVector3 camPos = ruVector3( Camera::msCurrentCamera->mView._41, Camera::msCurrentCamera->mView._42, Camera::msCurrentCamera->mView._43 );
	Engine::I().SetVertexShaderFloat3( 10, camPos.elements );
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

        vertexBuffer->Lock( 0, 0, &data, 0 );
        memcpy( data, vertices, sizeof( QuadVertex ) * 6 );
        vertexBuffer->Unlock( );
    }
}

EffectsQuad::EffectsQuad( bool bDebug ) {
	debug = bDebug;
	debugPixelShader = nullptr;
	Initialize();
	vertexShader = new VertexShader( "data/shaders/quad.vso" );
}

void EffectsQuad::Free()
{
	vertexDeclaration->Release();
	vertexBuffer->Release();
}

void EffectsQuad::Initialize()
{
	Engine::I().GetDevice()->CreateVertexBuffer( 6 * sizeof( QuadVertex ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vertexBuffer, 0 );

	if( !debug ) {
		QuadVertex vertices[ ] = {
			{           -0.5, -0.5, 0, 0, 0 }, { Engine::I().GetResolutionWidth() - 0.5,           - 0.5, 0, 1, 0 }, { -0.5, Engine::I().GetResolutionHeight() - 0.5, 0, 0, 1 },
			{  Engine::I().GetResolutionWidth() - 0.5, -0.5, 0, 1, 0 }, { Engine::I().GetResolutionWidth() - 0.5,  Engine::I().GetResolutionHeight() - 0.5, 0, 1, 1 }, { -0.5, Engine::I().GetResolutionHeight() - 0.5, 0, 0, 1 }
		};

		void * data = 0;
		
		vertexBuffer->Lock( 0, 0, &data, 0 );
		memcpy( data, vertices, sizeof( QuadVertex ) * 6 );
		vertexBuffer->Unlock( );
	} else {
		int size = 500;
		QuadVertex vertices[ ] = {
			{        -0.5, -0.5, 0, 0, 0 }, { size - 0.5,        -0.5, 0, 1, 0 }, { -0.5, size - 0.5, 0, 0, 1 },
			{  size - 0.5, -0.5, 0, 1, 0 }, { size - 0.5,  size - 0.5, 0, 1, 1 }, { -0.5, size - 0.5, 0, 0, 1 }
		};

		void * data = 0;

		vertexBuffer->Lock( 0, 0, &data, 0 );
		memcpy( data, vertices, sizeof( QuadVertex ) * 6 );
		vertexBuffer->Unlock( );

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

	Engine::I().GetDevice()->CreateVertexDeclaration( quadVertexDeclation, &vertexDeclaration );

	
	D3DXMatrixOrthoOffCenterLH ( &orthoProjection, 0, Engine::I().GetResolutionWidth(), Engine::I().GetResolutionHeight(), 0, 0, 1024 );
}
