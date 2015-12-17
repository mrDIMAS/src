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
#include "Engine.h"
#include "Skybox.h"
#include "Texture.h"
#include "Vertex.h"

Skybox::~Skybox() {
    OnLostDevice();
}

Skybox::Skybox( shared_ptr<Texture> up, shared_ptr<Texture> left, shared_ptr<Texture> right, shared_ptr<Texture> forward, shared_ptr<Texture> back ) {
	mTextures[0] = forward;
	mTextures[1] = back;
	mTextures[2] = right;
	mTextures[3] = left;
	mTextures[4] = up;
	OnResetDevice();
}

void Skybox::Render( ) {
	Engine::I().GetDevice()->SetVertexDeclaration( mVertexDeclaration );
    Engine::I().GetDevice()->SetIndices( mIndexBuffer );
	Engine::I().GetDevice()->SetStreamSource( 0, mVertexBuffer, 0, sizeof( SkyVertex ));
	for( int i = 0; i < 5; i++ ) {
		mTextures[i]->Bind( 3 );
		Engine::I().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 20, i * 6, 2 );
		Engine::I().RegisterDIP();
	}
}

void Skybox::OnLostDevice() {
	mVertexBuffer->Release();
	mIndexBuffer->Release();
	mVertexDeclaration->Release();
}

void Skybox::OnResetDevice() {	
	D3DVERTEXELEMENT9 vd[ ] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	Engine::I().GetDevice()->CreateVertexDeclaration( vd, &mVertexDeclaration );

	float size = 1024.0f;
	SkyVertex fv[ ] = { 
		{ -size,  size, -size, 0.0f, 0.0f },
		{  size,  size, -size, 1.0f, 0.0f },
		{  size, -size, -size, 1.0f, 1.0f },
		{ -size, -size, -size, 0.0f, 1.0f },
		{  size,  size,  size, 0.0f, 0.0f },
		{ -size,  size,  size, 1.0f, 0.0f },
		{ -size, -size,  size, 1.0f, 1.0f },
		{  size, -size,  size, 0.0f, 1.0f },
		{  size,  size, -size, 0.0f, 0.0f },
		{  size,  size,  size, 1.0f, 0.0f },
		{  size, -size,  size, 1.0f, 1.0f },
		{  size, -size, -size, 0.0f, 1.0f },
		{ -size,  size,  size, 0.0f, 0.0f },
		{ -size,  size, -size, 1.0f, 0.0f },
		{ -size, -size, -size, 1.0f, 1.0f },
		{ -size, -size,  size, 0.0f, 1.0f },
		{ -size,  size,  size, 0.0f, 0.0f },
		{  size,  size,  size, 1.0f, 0.0f },
		{  size,  size, -size, 1.0f, 1.0f },
		{ -size,  size, -size, 0.0f, 1.0f } };
	Engine::I().GetDevice()->CreateVertexBuffer( sizeof( fv ), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &mVertexBuffer, 0 );
	void * data = nullptr;
	mVertexBuffer->Lock( 0, 0, &data, 0 );
	memcpy( data, fv, sizeof( fv ));
	mVertexBuffer->Unlock();

	unsigned short indices [] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19 };
	Engine::I().GetDevice()->CreateIndexBuffer( sizeof( indices ), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mIndexBuffer, 0 );
	mIndexBuffer->Lock( 0, 0, &data, 0 );
	memcpy( data, indices, sizeof( indices ) );
	mIndexBuffer->Unlock();
}
