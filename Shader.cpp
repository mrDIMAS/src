#include "Precompiled.h"
#include "Engine.h"
#include "Shader.h"

vector<Shader*> Shader::msShaderList;
IDirect3DVertexShader9 * VertexShader::msLastBinded = nullptr;
IDirect3DPixelShader9 * PixelShader::msLastBinded = nullptr;

void VertexShader::Bind() {
	if( msLastBinded != shader ) {
		CheckDXErrorFatal( Engine::Instance().GetDevice()->SetVertexShader( shader ));
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
	Engine::Instance().GetDevice()->CreateVertexShader( code, &shader );
	delete code;
}

void VertexShader::OnResetDevice()
{
	Initialize();
}


void PixelShader::Bind() {
	if( msLastBinded != shader ) {
		CheckDXErrorFatal( Engine::Instance().GetDevice()->SetPixelShader( shader ));
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
	Engine::Instance().GetDevice()->CreatePixelShader( code, &shader );
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
		RaiseError( StringBuilder( "Failed to load shader " ) << mSourceName << " !" );
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
