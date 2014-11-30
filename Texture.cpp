#include "Texture.h"
#include "Utility.h"

map< string, Texture* > Texture::textures;

string g_texturePath = "data/textures/generic/";
Texture * Texture::Require( string file ) {
    Texture * texture = 0;

    auto existing = textures.find( file );

    if( existing == textures.end() ) {
		// texture not found in the storage, so create new one
        texture = new Texture;

        texture->texture = 0;
        texture->name = file;

		D3DXIMAGE_INFO imgInfo;

		if( FAILED( D3DXCreateTextureFromFileExA( g_device, file.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, &imgInfo, 0, &texture->texture ))) {
			LogMessage( Format( "Unable to load '%s' texture!", file.c_str() ));
		}
		
		texture->width = imgInfo.Width;
		texture->height = imgInfo.Height;
		texture->bpp = 32;

		textures[ file ] = texture;
    } else {
        texture = existing->second;
    }

    return texture;
}

Texture::Texture() {
	height = 0;
	width = 0;
	bpp = 0;
}

Texture::~Texture( ) {

}

void Texture::Bind( int level ) {
    CheckDXErrorFatal( g_device->SetTexture( level, texture ));
}

IDirect3DTexture9 * Texture::GetInterface() {
    return texture;
}

void Texture::DeleteAll() {
    for( auto tex : textures ) {
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
