#include "Texture.h"

map< string, Texture* > Texture::all;

Texture * Texture::Require( string file ) {
    Texture * texture = 0;

    map< string, Texture* >::iterator existing = all.find( file );

    if( existing == all.end() ) {
        texture = new Texture;

        texture->texture = 0;
        texture->name = file;

        D3DXCreateTextureFromFileA( g_device, file.c_str(), &texture->texture );

        all[ file ] = texture;
    } else {
        texture = existing->second;
    }

    return texture;
}

Texture::Texture() {

}

Texture::~Texture( ) {

}

void Texture::Bind( int level ) {
    CheckDXError( g_device->SetTexture( level, texture ));
}

IDirect3DTexture9 * Texture::GetInterface() {
    return texture;
}

void Texture::DeleteAll() {
    for( auto tex : all ) {
        if( tex.second->texture ) {
            tex.second->texture->Release();
        }
        delete tex.second;
    }
}

/////////////////////////////////////////////////////////////////
// API
TextureHandle TextureHandle::Empty() {
    TextureHandle handle;
    handle.pointer = 0;

    return handle;
}

TextureHandle GetTexture( const char * file ) {
    TextureHandle handle;
    handle.pointer = Texture::Require( file );
    return handle;
}
