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
    CheckDXErrorFatal( D3DXCreateCubeTextureFromFileA( g_device, fn.c_str(), &cubeTexture ));
}

CubeTexture::~CubeTexture() {
    cubeTexture->Release();
}

CubeTextureHandle GetCubeTexture( const char * file )
{
    CubeTextureHandle handle;
    handle.pointer = CubeTexture::Require( file );
    return handle;
}