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
#include "Texture.h"
#include "Utility.h"

unordered_map<string, weak_ptr<Texture>> Texture::msTextureList;

bool DirExists(const std::string& dirName_in) {
	return GetFileAttributesA(dirName_in.c_str()) & FILE_ATTRIBUTE_DIRECTORY;
}

shared_ptr<Texture> Texture::Request( string file ) {
    shared_ptr<Texture> pTexture;
    auto existing = msTextureList.find( file );
	if( existing != msTextureList.end() ) {
		pTexture = existing->second.lock();
	} 
	if( pTexture ) {
		return pTexture;
	} else {
		pTexture = make_shared<Texture>();
		pTexture->mName = file;
		if( pTexture->LoadFromFile( file )) {
			msTextureList[ file ] = pTexture;			
		} else {
			pTexture.reset();
		}
	}
    return std::move( pTexture );
}

Texture::Texture() :  mHeight( 0 ), mWidth( 0 ), mColorDepth( 0 ) {
	mResetPriority = RendererComponent::ResetPriority::High;
}

Texture::~Texture( ) {
	OnLostDevice();
}

IDirect3DTexture9 * Texture::GetInterface() {
    return mTexture;
}

int Texture::GetWidth() {
	return mWidth;
}

int Texture::GetHeight() {
	return mHeight;
}

void Texture::OnLostDevice() {
	mTexture.Reset();	
}

void Texture::OnResetDevice() {
	LoadFromFile( mName );
}

bool Texture::LoadFromFile( const string & file )
{
	D3DXIMAGE_INFO imgInfo;

	int slashPos = file.find_last_of( '/' );
	int dotPos = file.find_last_of( '.' );
	string ext = file.substr( dotPos + 1, file.size() - dotPos );
	string name = file.substr( slashPos + 1, dotPos - slashPos - 1 );

	// cache lookup
	string cacheFileName = "./cache/" + name + ".dds";
	FILE * pFile = fopen( cacheFileName.c_str(), "r" );
	bool genericTex = file.find( "textures/generic" ) != string::npos;
	if(pFile && genericTex ) {       
		fclose(pFile);
		if( FAILED( D3DXCreateTextureFromFileExA( pD3D, cacheFileName.c_str(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0, D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &imgInfo, NULL, &mTexture ))) {
			Log::Write( StringBuilder( "WARNING: Unable to load " ) << file << " texture!" );
			return false;
		} else {
			Log::Write( StringBuilder( "Texture successfully loaded from DDS cache: ") << cacheFileName );
		}
	} else {
		if( FAILED( D3DXCreateTextureFromFileExA( pD3D, file.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, &imgInfo, 0, &mTexture ))) {
			Log::Write( StringBuilder( "WARNING: Unable to load " ) << file << " texture!" );
			return false;
		} else {
			Log::Write( StringBuilder( "Texture successfully loaded: ") << file );
		}
	}
	mWidth = imgInfo.Width;
	mHeight = imgInfo.Height;
	mColorDepth = 32;
	return true;
}

std::string Texture::GetName() {
	return mName;
}

int Texture::GetColorDepth()
{
	return mColorDepth;
}

shared_ptr<ruTexture> ruTexture::Request( const string & file ) {
	return Texture::Request( file );
}

ruTexture::~ruTexture() {

}