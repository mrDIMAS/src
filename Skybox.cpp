#include "Precompiled.h"
#include "Engine.h"
#include "Skybox.h"
#include "Texture.h"
#include "Vertex.h"

Skybox::~Skybox() {
    OnLostDevice();
}

Skybox::Skybox( Texture * up, Texture * left, Texture * right, Texture * forward, Texture * back ) {
	mTextures[0] = forward;
	mTextures[1] = back;
	mTextures[2] = right;
	mTextures[3] = left;
	mTextures[4] = up;
	OnResetDevice();
}

void Skybox::Render( ) {
	Engine::Instance().GetDevice()->SetVertexDeclaration( mVertexDeclaration );
    Engine::Instance().GetDevice()->SetIndices( mIndexBuffer );
	Engine::Instance().GetDevice()->SetStreamSource( 0, mVertexBuffer, 0, sizeof( SkyVertex ));
	for( int i = 0; i < 5; i++ ) {
		mTextures[i]->Bind( 3 );
		Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 20, i * 6, 2 );
		Engine::Instance().RegisterDIP();
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
	Engine::Instance().GetDevice()->CreateVertexDeclaration( vd, &mVertexDeclaration );

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
	Engine::Instance().GetDevice()->CreateVertexBuffer( sizeof( fv ), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &mVertexBuffer, 0 );
	void * data = nullptr;
	mVertexBuffer->Lock( 0, 0, &data, 0 );
	memcpy( data, fv, sizeof( fv ));
	mVertexBuffer->Unlock();

	unsigned short indices [] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19 };
	Engine::Instance().GetDevice()->CreateIndexBuffer( sizeof( indices ), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mIndexBuffer, 0 );
	mIndexBuffer->Lock( 0, 0, &data, 0 );
	memcpy( data, indices, sizeof( indices ) );
	mIndexBuffer->Unlock();
}
