#include "Shader.h"




void VertexShader::Bind() {
    g_device->SetVertexShader( shader );
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

    g_device->CreateVertexShader ( ( DWORD * ) buffer->GetBufferPointer(), &shader );
}

void PixelShader::Bind() {
    g_device->SetPixelShader( shader );
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

    g_device->CreatePixelShader( ( DWORD * ) buffer->GetBufferPointer(), &shader );
}
