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
    explicit Renderer( int width, int height, int fullscreen );
    virtual ~Renderer();
	void RenderWorld( );
};