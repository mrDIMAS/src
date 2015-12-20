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

IDirect3DVertexShader9 * VertexShader::msLastBinded = nullptr;
IDirect3DPixelShader9 * PixelShader::msLastBinded = nullptr;

void VertexShader::Bind() {
	if( msLastBinded != shader ) {
		Engine::I().GetDevice()->SetVertexShader( shader );
		msLastBinded = shader;
	}
}

VertexShader::~VertexShader() {
    OnLostDevice();
}

VertexShader::VertexShader( string fileName ) : Shader( fileName ) {
    Initialize();
}

void VertexShader::OnLostDevice() {
	shader.Reset();
}

void VertexShader::Initialize() {
	DWORD * code = LoadBinary();
	if( SUCCEEDED( Engine::I().GetDevice()->CreateVertexShader( code, &shader ))) {
		Log::Write( StringBuilder( "New vertex shader successfully created from ") << mSourceName << " binary!" );
	} else {
		Log::Error( StringBuilder( "Unable to create vertex shader from ") << mSourceName << " binary!" );
	}
	delete code;
}

void VertexShader::OnResetDevice() {
	Initialize();
}


void PixelShader::Bind() {
	if( msLastBinded != shader ) {
		Engine::I().GetDevice()->SetPixelShader( shader );
		msLastBinded = shader;
	}
}

PixelShader::~PixelShader() {
    OnLostDevice();
}

PixelShader::PixelShader( string fileName ) : Shader( fileName )  {
	Initialize();
}

void PixelShader::OnLostDevice() {
	shader.Reset();	
}

void PixelShader::Initialize() {
	DWORD * code = LoadBinary();
	if( SUCCEEDED( Engine::I().GetDevice()->CreatePixelShader( code, &shader ))) {
		Log::Write( StringBuilder( "New pixel shader successfully created from ") << mSourceName << " binary!" );
	} else {
		Log::Error( StringBuilder( "Unable to create pixel shader from ") << mSourceName << " binary!" );
	}
	delete code;
}

void PixelShader::OnResetDevice() {
	Initialize();
}

Shader::Shader( const string & sourceName ) : mSourceName( sourceName ) {

}

DWORD * Shader::LoadBinary() {
	UINT fSize = 0;
	DWORD * binaryData;
	ifstream pFile( mSourceName, ios_base::binary | ios_base::in );
	if( pFile.bad() ) {
		Log::Error( StringBuilder( "Failed to load shader " ) << mSourceName << " !" );
	}
	pFile.seekg( 0, ios_base::end );
	fSize = pFile.tellg();
	pFile.seekg( 0, ios_base::beg );
	binaryData = new DWORD[ fSize ];
	pFile.read( reinterpret_cast<char*>( binaryData ), fSize );
	pFile.close();
	return binaryData;
}

Shader::~Shader() {

}

void Shader::OnResetDevice() {

}

void Shader::OnLostDevice() {

}
