#pragma once

#include "Common.h"
#include "Timer.h"
#include "BitmapFont.h"

class Renderer {
public:
    static LRESULT CALLBACK WindowProcess ( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam );
    void UpdateMessagePump( );
    void RenderMeshesIntoGBuffer();    
    void CreatePhysics( );
    int CreateRenderWindow( int width, int height, int fullscreen );
    bool IsMeshVisible( Mesh * mesh );
    Timer * performanceTimer;
	HWND window;
public:
    explicit Renderer( int width, int height, int fullscreen, char vSync );
    virtual ~Renderer();
	void RenderWorld( );
	void SetVertexShaderMatrix( UINT startRegister, D3DMATRIX * matrix ) {
		g_device->SetVertexShaderConstantF( startRegister, &matrix->m[0][0], 4 );
	}
	void SetVertexShaderFloat3( UINT startRegister, float * v ) {
		float buffer[ 4 ];
		buffer[0] = v[0];
		buffer[1] = v[1];
		buffer[2] = v[2];
		buffer[3] = 0.0f;
		g_device->SetVertexShaderConstantF( startRegister, buffer, 1 );
	}
	void SetVertexShaderFloat( UINT startRegister, float v ) {
		float buffer[ 4 ];
		buffer[0] = v;
		buffer[1] = 0.0f;
		buffer[2] = 0.0f;
		buffer[3] = 0.0f;
		g_device->SetVertexShaderConstantF( startRegister, buffer, 1 );
	}
	void SetVertexShaderInt( UINT startRegister, int v ) {
		int buffer[ 4 ];
		buffer[0] = v;
		buffer[1] = 0;
		buffer[2] = 0;
		buffer[3] = 0;
		g_device->SetVertexShaderConstantI( startRegister, buffer, 1 );
	}
	void SetVertexShaderBool( UINT startRegister, BOOL v ) {
		BOOL buffer[ 4 ];
		buffer[0] = v;
		buffer[1] = FALSE;
		buffer[2] = FALSE;
		buffer[3] = FALSE;
		g_device->SetVertexShaderConstantB( startRegister, buffer, 1 );
	}

	void SetPixelShaderMatrix( UINT startRegister, D3DMATRIX * matrix ) {
		g_device->SetPixelShaderConstantF( startRegister, &matrix->m[0][0], 4 );
	}
	void SetPixelShaderFloat3( UINT startRegister, float * v ) {
		float buffer[ 4 ];
		buffer[0] = v[0];
		buffer[1] = v[1];
		buffer[2] = v[2];
		buffer[3] = 0.0f;
		g_device->SetPixelShaderConstantF( startRegister, buffer, 1 );
	}
	void SetPixelShaderFloat( UINT startRegister, float v ) {
		float buffer[ 4 ];
		buffer[0] = v;
		buffer[1] = 0.0f;
		buffer[2] = 0.0f;
		buffer[3] = 0.0f;
		g_device->SetPixelShaderConstantF( startRegister, buffer, 1 );
	}
	void SetPixelShaderInt( UINT startRegister, int v ) {
		int buffer[ 4 ];
		buffer[0] = v;
		buffer[1] = 0;
		buffer[2] = 0;
		buffer[3] = 0;
		g_device->SetPixelShaderConstantI( startRegister, buffer, 1 );
	}
	void SetPixelShaderBool( UINT startRegister, BOOL v ) {
		BOOL buffer[ 4 ];
		buffer[0] = v;
		buffer[1] = FALSE;
		buffer[2] = FALSE;
		buffer[3] = FALSE;
		g_device->SetPixelShaderConstantB( startRegister, buffer, 1 );
	}
};