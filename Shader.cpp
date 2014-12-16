#include "Shader.h"

void VertexShader::Bind() {
    CheckDXErrorFatal( g_pDevice->SetVertexShader( shader ));
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

VertexShader::VertexShader( string source ) {
    ID3DXBuffer * buffer = 0;
    ID3DXBuffer * errMessages = 0;
    DWORD flags = D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_OPTIMIZATION_LEVEL3;

    if( FAILED( D3DXCompileShader ( source.c_str(), source.size(), 0, 0, "main", "vs_3_0", flags, &buffer, &errMessages, &constants ))) {
        MessageBoxA( 0, ( const char * ) errMessages->GetBufferPointer(), "Error", MB_OK ) ;

        exit( -1 );
    }

    CheckDXErrorFatal( g_pDevice->CreateVertexShader ( ( DWORD * ) buffer->GetBufferPointer(), &shader ));
}

VertexShader::VertexShader( string fileName, bool binary ) {
    FILE * pFile = 0;
    BYTE * shaderCode = 0;
    UINT fSize = 0;
    UINT numRead = 0;

    pFile = fopen( fileName.c_str(), "rb" );

    fseek(pFile, 0, SEEK_END);
    fSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    shaderCode = (BYTE*)malloc(fSize);
    while (numRead != fSize) {
        numRead = fread(&shaderCode[numRead], 1, fSize, pFile);
    }
    fclose(pFile);
    CheckDXErrorFatal( g_pDevice->CreateVertexShader ( ( DWORD*)shaderCode, &shader ));
    constants = 0;
}


void PixelShader::Bind() {
    CheckDXErrorFatal( g_pDevice->SetPixelShader( shader ));
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

PixelShader::PixelShader( string source ) {
    ID3DXBuffer * buffer = 0;
    ID3DXBuffer * errMessages = 0;
    DWORD flags = D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_OPTIMIZATION_LEVEL3;

    if( FAILED( D3DXCompileShader ( source.c_str(), source.size(), 0, 0, "main", "ps_3_0", flags, &buffer, &errMessages, &constants ))) {
        MessageBoxA( 0, ( const char * ) errMessages->GetBufferPointer(), "Error", MB_OK ) ;

        exit( -1 );
    }

    CheckDXErrorFatal( g_pDevice->CreatePixelShader( ( DWORD * ) buffer->GetBufferPointer(), &shader ));
}


PixelShader::PixelShader( string fileName, bool binary ) {
    FILE * pFile = 0;
    BYTE * shaderCode = 0;
    UINT fSize = 0;
    UINT numRead = 0;

    pFile = fopen( fileName.c_str(), "rb" );

    fseek(pFile, 0, SEEK_END);
    fSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    shaderCode = (BYTE*)malloc(fSize);
    while (numRead != fSize) {
        numRead = fread(&shaderCode[numRead], 1, fSize, pFile);
    }
    fclose(pFile);
    CheckDXErrorFatal( g_pDevice->CreatePixelShader ( ( DWORD*)shaderCode, &shader ));
    constants = 0;
}
