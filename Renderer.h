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
    void SetVertexShaderMatrix( UINT startRegister, D3DMATRIX * matrix );
    void SetVertexShaderFloat3( UINT startRegister, float * v );
    void SetVertexShaderFloat( UINT startRegister, float v );
    void SetVertexShaderInt( UINT startRegister, int v );
    void SetVertexShaderBool( UINT startRegister, BOOL v );
    void SetPixelShaderMatrix( UINT startRegister, D3DMATRIX * matrix );
    void SetPixelShaderFloat3( UINT startRegister, float * v );
    void SetPixelShaderFloat( UINT startRegister, float v );
    void SetPixelShaderInt( UINT startRegister, int v );
    void SetPixelShaderBool( UINT startRegister, BOOL v );
};