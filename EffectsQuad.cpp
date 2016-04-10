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
	mVertexShader->Bind();
	pD3D->SetVertexShaderConstantF( 0, &mOrthoProjection.m[0][0], 4 );		
    pD3D->SetStreamSource( 0, mVertexBuffer, 0, sizeof( QuadVertex ));
    pD3D->SetVertexDeclaration( mVertexDeclaration );
    pD3D->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
}

EffectsQuad::~EffectsQuad() {
    Free();
}

EffectsQuad::EffectsQuad(  ) {
	Initialize();
	mVertexShader = unique_ptr<VertexShader>( new VertexShader( "data/shaders/quad.vso" ));
}

void EffectsQuad::Free() {
	mVertexDeclaration.Reset();
	mVertexBuffer.Reset();
}

void EffectsQuad::Initialize()
{
	void * data;
	
	D3DVERTEXELEMENT9 quadVertexDeclation[ ] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	QuadVertex vertices[ ] = {
		{ -0.5, -0.5, 0, 0, 0 }, 
		{ pEngine->GetResolutionWidth() - 0.5, -0.5, 0, 1, 0 }, 
		{ -0.5, pEngine->GetResolutionHeight() - 0.5, 0, 0, 1 },
		{  pEngine->GetResolutionWidth() - 0.5, -0.5, 0, 1, 0 }, 
		{ pEngine->GetResolutionWidth() - 0.5, pEngine->GetResolutionHeight() - 0.5, 0, 1, 1 }, 
		{ -0.5, pEngine->GetResolutionHeight() - 0.5, 0, 0, 1 }
	};

	pD3D->CreateVertexBuffer( 6 * sizeof( QuadVertex ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &mVertexBuffer, 0 );
	mVertexBuffer->Lock( 0, 0, &data, 0 );
	memcpy( data, vertices, sizeof( QuadVertex ) * 6 );
	mVertexBuffer->Unlock( );
	
	pD3D->CreateVertexDeclaration( quadVertexDeclation, &mVertexDeclaration );
		
	D3DXMatrixOrthoOffCenterLH ( &mOrthoProjection, 0, pEngine->GetResolutionWidth(), pEngine->GetResolutionHeight(), 0, 0, 1024 );
}

void EffectsQuad::OnLostDevice()
{
	Free();
}

void EffectsQuad::OnResetDevice()
{
	Initialize();
}
