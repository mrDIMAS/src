#include "Engine.h"

#define INITGUID

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )

#include <windows.h>
#include <dinput.h>

namespace mi {

LPDIRECTINPUT8			pInput;
LPDIRECTINPUTDEVICE8	pKeyboard;
LPDIRECTINPUTDEVICE8	pMouse;
DIMOUSESTATE			mouseData;
HWND					hwnd;

char g_keys[ 256 ]		= { 0 };
char g_lastKeys[ 256 ]	= { 0 };
char g_keysHit[ 256 ] = { 0 };
char g_keysUp[ 256 ] = { 0 };

char g_mouseUp[ 4 ] = { 0 };
char g_mouseHit[ 4 ] = { 0 };
char g_mousePressed[ 4 ] = { 0 };

int libraryUsed			= 0;
int mouseX				= 0;
int mouseY				= 0;
int mouseWheel			= 0;
int windowOffsetX		= 0;
int windowOffsetY		= 0;
RECT windowRect;

void Update( ) {
    if( pKeyboard->GetDeviceState( sizeof( g_keys ),( void * )&g_keys ) == DIERR_INPUTLOST ) {
        pKeyboard->Acquire();
    }

    if( pMouse->GetDeviceState( sizeof( DIMOUSESTATE ),( void * )&mouseData ) == DIERR_INPUTLOST ) {
        pMouse->Acquire();
    }

    for( size_t i = 0; i < sizeof( g_keys ); ++i ) {
        g_keysHit[ i ] = g_keys[ i ] & ~g_lastKeys[ i ];
        g_keysUp[ i ]  = ~g_keys[ i ] & g_lastKeys[ i ];
    }

    for( size_t i = 0; i < 4; ++i ) {
        g_mouseUp[ i ] =  mouseData.rgbButtons[ i ] & ~g_mousePressed[ i ];
        g_mouseHit[ i ] = ~mouseData.rgbButtons[ i ] & g_mousePressed[ i ];
    }

    mouseX += mouseData.lX;
    mouseY += mouseData.lY;
    mouseWheel += mouseData.lZ;

    if( mouseX < 0 )
        mouseX = 0;
    if( mouseY < 0 )
        mouseY = 0;
    if( mouseX > windowRect.right )
        mouseX = windowRect.right;
    if( mouseY > windowRect.bottom )
        mouseY = windowRect.bottom;

    SetCursorPos( windowOffsetX + windowRect.left + mouseX, windowOffsetY + windowRect.top + mouseY );

    memcpy( g_lastKeys, g_keys, sizeof( g_lastKeys ));
    memcpy( g_mousePressed, mouseData.rgbButtons, sizeof( g_mousePressed ));
};

Message Init( void * window ) {
    HINSTANCE hInstance = GetModuleHandle( 0 );

    hwnd = (HWND)(*(HWND*)window);

    if( FAILED( DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pInput,NULL)))
        return UnableToInitializeDirectInput;

    if( FAILED( pInput->CreateDevice( GUID_SysKeyboard, &pKeyboard, NULL )))
        return UnableToCreateKeyboardDevice;

    pKeyboard->SetDataFormat( &c_dfDIKeyboard );
    pKeyboard->SetCooperativeLevel( 0, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
    pKeyboard->Acquire();

    if( FAILED( pInput->CreateDevice( GUID_SysMouse, &pMouse, NULL )))
        return UnableToCreateMouseDevice;

    pMouse->SetDataFormat(&c_dfDIMouse);
    pMouse->SetCooperativeLevel( 0, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
    pMouse->Acquire();

    RECT initialWindowRect;
    GetWindowRect( hwnd, &initialWindowRect );
    GetClientRect( hwnd, &windowRect );
    AdjustWindowRect( &windowRect, GetWindowLong( hwnd, GWL_STYLE ), 0 );
    windowOffsetX = initialWindowRect.left - windowRect.left;
    windowOffsetY = initialWindowRect.top - windowRect.top ;
    GetClientRect( hwnd, &windowRect );

    return DeviceInitializedSuccessfully;
};

Message Destroy( ) {
    libraryUsed = 0;

    if( pMouse ) {
        pMouse->Unacquire();

        pMouse->Release();
    }

    if( pKeyboard ) {
        pKeyboard->Unacquire();
        pKeyboard->Release();
    }

    if( pInput ) {
        pInput->Release();
    }

    return DeviceDestroyedSuccessfully;
};

int KeyDown( Key key ) {
    return g_keys[ key ];
};

int KeyHit( Key key ) {
    return g_keysHit[ key ];
};

int	KeyUp	( Key key ) {
    return g_keysUp[ key ];
}

int MouseDown( MouseButton button ) {
    if( (int)button >= 4 )
        return 0;

    return mouseData.rgbButtons[ button ];
}

int MouseHit( MouseButton button ) {
    return g_mouseHit[ button ];
}

int MouseX( ) {
    return mouseX;
}

int MouseY( ) {
    return mouseY;
}

int MouseWheel( ) {
    return mouseWheel;
}

int MouseXSpeed( ) {
    return mouseData.lX;
}

int MouseYSpeed( ) {
    return mouseData.lY;
}

int MouseWheelSpeed( ) {
    return mouseData.lZ;
}

};