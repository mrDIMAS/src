#include "Texture.h"
#include "Utility.h"

unordered_map< string, Texture* > Texture::msTextureList;

string g_texturePath = "data/textures/generic/";
Texture * Texture::Require( string file )
{
    Texture * pTexture = 0;

    auto existing = msTextureList.find( file );

    if( existing == msTextureList.end() )
    {
        // texture not found in the storage, so create new one
        pTexture = new Texture;

        pTexture->texture = 0;
        pTexture->name = file;

        D3DXIMAGE_INFO imgInfo;

        int slashPos = file.find_last_of( '/' );
        int dotPos = file.find_last_of( '.' );
        string ext = file.substr( dotPos + 1, file.size() - dotPos );
        string name = file.substr( slashPos + 1, dotPos - slashPos - 1 );

        // cache lookup
        string cacheFileName = g_texturePath + "cache/" + name + ".dds";
        FILE * pFile = fopen( cacheFileName.c_str(), "r" );
        if( pFile )   // got cached DXT5 texture
        {
            fclose( pFile );
            if( FAILED( D3DXCreateTextureFromFileExA( g_pDevice, cacheFileName.c_str(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0, D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &imgInfo, NULL, &pTexture->texture )))
                LogMessage( Format( "Unable to load '%s' texture!", file.c_str() ));
        }
        else
        {
            if( FAILED( D3DXCreateTextureFromFileExA( g_pDevice, file.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, &imgInfo, 0, &pTexture->texture )))
                LogMessage( Format( "Unable to load '%s' texture!", file.c_str() ));
        }
        pTexture->width = imgInfo.Width;
        pTexture->height = imgInfo.Height;
        pTexture->bpp = 32;
        msTextureList[ file ] = pTexture;
    }
    else
        pTexture = existing->second;

    return pTexture;
}

Texture::Texture()
{
    height = 0;
    width = 0;
    bpp = 0;
}

Texture::~Texture( )
{

}

void Texture::Bind( int level )
{
    CheckDXErrorFatal( g_pDevice->SetTexture( level, texture ));
}

IDirect3DTexture9 * Texture::GetInterface()
{
    return texture;
}

void Texture::DeleteAll()
{
    for( auto tex : msTextureList )
    {
        if( tex.second->texture )
            tex.second->texture->Release();
        delete tex.second;
    }
}

/////////////////////////////////////////////////////////////////
// API
ruTextureHandle ruTextureHandle::Empty()
{
    ruTextureHandle handle;
    handle.pointer = 0;

    return handle;
}

ruTextureHandle ruGetTexture( const char * file )
{
    ruTextureHandle handle;
    handle.pointer = Texture::Require( file );
    return handle;
}
