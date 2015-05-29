#include "Precompiled.h"
#include "Engine.h"
#include "Shader.h"

IDirect3DVertexShader9 * VertexShader::msLastBinded = nullptr;
IDirect3DPixelShader9 * PixelShader::msLastBinded = nullptr;

void VertexShader::Bind() {
	if( msLastBinded != shader ) {
		CheckDXErrorFatal( Engine::Instance().GetDevice()->SetVertexShader( shader ));
		msLastBinded = shader;
	}
}

ID3DXConstantTable * VertexShader::GetConstantTable() {
    return constants;
}

VertexShader::~VertexShader() {
    if( shader ) {
        shader->Release();
    }
    if( constants ) {
        constants->Release();
    }
}

VertexShader::VertexShader( string fileName ) {
    FILE * pFile = 0;
    BYTE * shaderCode = 0;
    UINT fSize = 0;
    UINT numRead = 0;

    pFile = fopen( fileName.c_str(), "rb" );

	if( !pFile ) {
		RaiseError( StringBuilder( "Failed to load shader " ) << fileName << " !" );
	}

    fseek(pFile, 0, SEEK_END);
    fSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    shaderCode = new BYTE[fSize];
    while (numRead != fSize) {
        numRead = fread(&shaderCode[numRead], 1, fSize, pFile);
    }
    fclose(pFile);
    CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateVertexShader ( ( DWORD*)shaderCode, &shader ));
    constants = 0;
}


void PixelShader::Bind() {
	if( msLastBinded != shader ) {
		CheckDXErrorFatal( Engine::Instance().GetDevice()->SetPixelShader( shader ));
		msLastBinded = shader;
	}
}

ID3DXConstantTable * PixelShader::GetConstantTable() {
    return constants;
}

PixelShader::~PixelShader() {
    if( shader ) {
        shader->Release();
    }
    if( constants ) {
        constants->Release();
    }
}

PixelShader::PixelShader( string fileName ) {
    FILE * pFile = 0;
    BYTE * shaderCode = 0;
    UINT fSize = 0;
    UINT numRead = 0;

    pFile = fopen( fileName.c_str(), "rb" );

	if( !pFile ) {
		RaiseError( StringBuilder( "Failed to load shader " ) << fileName << " !" );
	}

    fseek(pFile, 0, SEEK_END);
    fSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    shaderCode = new BYTE[fSize];
    while (numRead != fSize) {
        numRead = fread(&shaderCode[numRead], 1, fSize, pFile);
    }
    fclose(pFile);
    CheckDXErrorFatal( Engine::Instance().GetDevice()->CreatePixelShader ( ( DWORD*)shaderCode, &shader ));
    constants = 0;
}
