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
    CheckDXErrorFatal( D3DXCreateCubeTextureFromFileA( gpDevice, fn.c_str(), &cubeTexture ));
}

CubeTexture::~CubeTexture() {
    cubeTexture->Release();
}

ruCubeTextureHandle ruGetCubeTexture( const char * file ) {
    ruCubeTextureHandle handle;
    handle.pointer = CubeTexture::Require( file );
    return handle;
}