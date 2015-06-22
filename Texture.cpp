#include "Precompiled.h"
#include "Engine.h"
#include "Texture.h"
#include "Utility.h"
#include <sys/stat.h>

unordered_map< string, Texture* > Texture::msTextureList;

bool DirExists(const std::string& dirName_in)
{
	return GetFileAttributesA(dirName_in.c_str()) & FILE_ATTRIBUTE_DIRECTORY;
}


Texture * Texture::Require( string file ) {
	if( !DirExists( "./cache/" )) {
		Log::Error( "You must rebuild texture cache!" );
	}

    Texture * pTexture = 0;

    auto existing = msTextureList.find( file );

    if( existing == msTextureList.end() ) {
        // texture not found in the storage, so create new one
        pTexture = new Texture;

        pTexture->mTexture = 0;
        pTexture->mName = file;

		pTexture->LoadFromFile( file );

        msTextureList[ file ] = pTexture;
    } else {
        pTexture = existing->second;
    }

    return pTexture;
}

Texture::Texture() {
    mHeight = 0;
    mWidth = 0;
    mColorDepth = 0;
	mResetPriority = RendererComponent::ResetPriority::High;
}

Texture::~Texture( ) {

}

void Texture::Bind( int level ) {
    Engine::Instance().GetDevice()->SetTexture( level, mTexture );
}

IDirect3DTexture9 * Texture::GetInterface() {
    return mTexture;
}

void Texture::DeleteAll() {
    for( auto tex : msTextureList ) {
        if( tex.second->mTexture ) {
            tex.second->mTexture->Release();
        }
        delete tex.second;
    }
}

int Texture::GetWidth()
{
	return mWidth;
}

int Texture::GetHeight()
{
	return mHeight;
}

void Texture::OnLostDevice()
{
	if( mTexture ) {
		mTexture->Release();
	}
}

void Texture::OnResetDevice()
{
	LoadFromFile( mName );
}

void Texture::LoadFromFile( const string & file )
{
	D3DXIMAGE_INFO imgInfo;

	int slashPos = file.find_last_of( '/' );
	int dotPos = file.find_last_of( '.' );
	string ext = file.substr( dotPos + 1, file.size() - dotPos );
	string name = file.substr( slashPos + 1, dotPos - slashPos - 1 );

	// cache lookup
	string cacheFileName = "./cache/" + name + ".dds";
	FILE * pFile = fopen( cacheFileName.c_str(), "r" );
	if(pFile) {       
		fclose(pFile);
		if( FAILED( D3DXCreateTextureFromFileExA( Engine::Instance().GetDevice(), cacheFileName.c_str(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0, D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &imgInfo, NULL, &mTexture ))) {
			Log::Write( StringBuilder( "Unable to load " ) << file << " texture!" );
		}
	} else {
		if( FAILED( D3DXCreateTextureFromFileExA( Engine::Instance().GetDevice(), file.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, &imgInfo, 0, &mTexture ))) {
			Log::Write( StringBuilder( "Unable to load " ) << file << " texture!" );
		}
	}
	mWidth = imgInfo.Width;
	mHeight = imgInfo.Height;
	mColorDepth = 32;
}

/////////////////////////////////////////////////////////////////
// API
ruTextureHandle ruTextureHandle::Empty() {
    ruTextureHandle handle;
    handle.pointer = 0;

    return handle;
}

ruTextureHandle ruGetTexture( const string & file ) {
    ruTextureHandle handle;
    handle.pointer = Texture::Require( file );
    return handle;
}

int ruGetTextureWidth( ruTextureHandle texture ) {
	return ((Texture*)texture.pointer)->GetWidth();
}

int ruGetTextureHeight( ruTextureHandle texture ) {
	return ((Texture*)texture.pointer)->GetHeight();
}
