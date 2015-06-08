#include "Precompiled.h"
#include "Engine.h"
#include "CubeTexture.h"

map< string, CubeTexture*> CubeTexture::all;

CubeTexture * CubeTexture::Require( string fn ) {
    auto existing = all.find( fn );
    if( existing != all.end() ) {
        return existing->second;
    } else {
        CubeTexture * newCubeTexture = new CubeTexture( fn );
        all[ fn ] = newCubeTexture;
        return newCubeTexture;
    }
}

CubeTexture::CubeTexture( string fn ) {
	mSourceName = fn;
	Load();
}

CubeTexture::~CubeTexture() {
    cubeTexture->Release();
}

void CubeTexture::OnLostDevice()
{
	cubeTexture->Release();
}

void CubeTexture::OnResetDevice()
{
	Load();
}

void CubeTexture::Load()
{
	D3DXCreateCubeTextureFromFileA( Engine::Instance().GetDevice(), mSourceName.c_str(), &cubeTexture );
}

ruCubeTextureHandle ruGetCubeTexture( const string & file ) {
    ruCubeTextureHandle handle;
    handle.pointer = CubeTexture::Require( file );
    return handle;
}