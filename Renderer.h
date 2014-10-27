#pragma once

#include "Common.h"
#include "Timer.h"

class Renderer {
public:
    enum {
        TypeDeferredRenderer,
        TypeLightMapRenderer,
    };

    static LRESULT CALLBACK WindowProcess ( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam );
    void UpdateMessagePump( );
    void RenderMeshesIntoGBuffer();
    void RenderWorld( );
    void CreatePhysics( );
    int CreateRenderWindow( int width, int height, int fullscreen );
    bool IsMeshVisible( Mesh * mesh );
    Timer * performanceTimer;
public:
    Renderer( int width, int height, int fullscreen );
    ~Renderer();
};