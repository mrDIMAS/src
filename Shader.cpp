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
#include "Shader.h"

vector<Shader*> Shader::msShaderList;
IDirect3DVertexShader9 * VertexShader::msLastBinded = nullptr;
IDirect3DPixelShader9 * PixelShader::msLastBinded = nullptr;

void VertexShader::Bind() {
	if( msLastBinded != shader ) {
		Engine::Instance().GetDevice()->SetVertexShader( shader );
		msLastBinded = shader;
	}
}

VertexShader::~VertexShader() {
    if( shader ) {
        shader->Release();
    }
}

VertexShader::VertexShader( string fileName ) : Shader( fileName ) {
    Initialize();
}

void VertexShader::OnLostDevice() {
	shader->Release();
}

void VertexShader::Initialize()
{
	DWORD * code = LoadBinary();
	if( SUCCEEDED( Engine::Instance().GetDevice()->CreateVertexShader( code, &shader ))) {
		Log::Write( StringBuilder( "New vertex shader successfully created from ") << mSourceName << " binary!" );
	} else {
		Log::Error( StringBuilder( "Unable to create vertex shader from ") << mSourceName << " binary!" );
	}
	delete code;
}

void VertexShader::OnResetDevice()
{
	Initialize();
}


void PixelShader::Bind() {
	if( msLastBinded != shader ) {
		Engine::Instance().GetDevice()->SetPixelShader( shader );
		msLastBinded = shader;
	}
}

PixelShader::~PixelShader() {
    if( shader ) {
        shader->Release();
    }
}

PixelShader::PixelShader( string fileName ) : Shader( fileName )  {
	Initialize();
}

void PixelShader::OnLostDevice() {
	shader->Release();	
}

void PixelShader::Initialize()
{
	DWORD * code = LoadBinary();
	if( SUCCEEDED( Engine::Instance().GetDevice()->CreatePixelShader( code, &shader ))) {
		Log::Write( StringBuilder( "New pixel shader successfully created from ") << mSourceName << " binary!" );
	} else {
		Log::Error( StringBuilder( "Unable to create pixel shader from ") << mSourceName << " binary!" );
	}
	delete code;
}

void PixelShader::OnResetDevice()
{
	Initialize();
}

Shader::Shader( const string & sourceName ) : mSourceName( sourceName ) {
	msShaderList.push_back( this );
}

DWORD * Shader::LoadBinary() {
	FILE * pFile = 0;
	UINT fSize = 0;
	UINT numRead = 0;
	DWORD * binaryData;
	pFile = fopen( mSourceName.c_str(), "rb" );

	if( !pFile ) {
		Log::Error( StringBuilder( "Failed to load shader " ) << mSourceName << " !" );
	}

	fseek(pFile, 0, SEEK_END);
	fSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	binaryData = new DWORD[ fSize ];
	while (numRead != fSize) {
		numRead = fread( &binaryData[numRead], 1, fSize, pFile);
	}
	fclose(pFile);
	return binaryData;
}

Shader::~Shader()
{
	msShaderList.erase( find( msShaderList.begin(), msShaderList.end(), this ));
}

void Shader::OnResetDevice()
{

}

void Shader::OnLostDevice()
{

}
