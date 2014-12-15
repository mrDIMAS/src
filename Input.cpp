#include "Common.h"
#include "GUIButton.h"

LPDIRECTINPUT8			pInput;
LPDIRECTINPUTDEVICE8	pKeyboard;
LPDIRECTINPUTDEVICE8	pMouse;
DIMOUSESTATE			mouseData;
HWND					hwnd;

char g_keys[ 256 ] = { 0 };
char g_lastKeys[ 256 ] = { 0 };
char g_keysHit[ 256 ] = { 0 };
char g_keysUp[ 256 ] = { 0 };

char g_mouseUp[ 4 ] = { 0 };
char g_mouseHit[ 4 ] = { 0 };
char g_mousePressed[ 4 ] = { 0 };

int mouseX = 0;
int mouseY = 0;
int mouseWheel = 0;
int windowOffsetX = 0;
int windowOffsetY = 0;
RECT windowRect;

void ruInputUpdate( )
{


    if( pKeyboard->GetDeviceState( sizeof( g_keys ),( void * )&g_keys ) == DIERR_INPUTLOST )
        pKeyboard->Acquire();

    if( pMouse->GetDeviceState( sizeof( DIMOUSESTATE ),( void * )&mouseData ) == DIERR_INPUTLOST )
        pMouse->Acquire();

    for( size_t i = 0; i < sizeof( g_keys ); ++i )
    {
        g_keysHit[ i ] = g_keys[ i ] & ~g_lastKeys[ i ];
        g_keysUp[ i ]  = ~g_keys[ i ] & g_lastKeys[ i ];
    }

    for( size_t i = 0; i < 4; ++i )
    {
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

    //SetCursorPos( windowOffsetX + windowRect.left + mouseX, windowOffsetY + windowRect.top + mouseY );

    memcpy( g_lastKeys, g_keys, sizeof( g_lastKeys ));
    memcpy( g_mousePressed, mouseData.rgbButtons, sizeof( g_mousePressed ));

	for( auto pButton : GUIButton::msButtonList )
	{
		if( pButton->IsVisible() )
		{
			pButton->Update();
		}		
	}
};

RUAPI void ruInputInit( void * window )
{
    HINSTANCE hInstance = GetModuleHandle( 0 );
    hwnd = (HWND)(*(HWND*)window);
    CheckDXErrorFatal( DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pInput,NULL));
    CheckDXErrorFatal( pInput->CreateDevice( GUID_SysKeyboard, &pKeyboard, NULL ));
    CheckDXErrorFatal( pKeyboard->SetDataFormat( &c_dfDIKeyboard ));
    CheckDXErrorFatal( pKeyboard->SetCooperativeLevel( hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE ));
    CheckDXErrorFatal( pKeyboard->Acquire());
    CheckDXErrorFatal( pInput->CreateDevice( GUID_SysMouse, &pMouse, NULL ));
    CheckDXErrorFatal( pMouse->SetDataFormat(&c_dfDIMouse));
    CheckDXErrorFatal( pMouse->SetCooperativeLevel( hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE ));
    CheckDXErrorFatal( pMouse->Acquire());
    RECT initialWindowRect;
    GetWindowRect( hwnd, &initialWindowRect );
    GetClientRect( hwnd, &windowRect );
    AdjustWindowRect( &windowRect, GetWindowLong( hwnd, GWL_STYLE ), 0 );
    windowOffsetX = initialWindowRect.left - windowRect.left;
    windowOffsetY = initialWindowRect.top - windowRect.top ;
    GetClientRect( hwnd, &windowRect );
};

RUAPI void ruInputDestroy()
{
    CheckDXErrorFatal( pMouse->Unacquire());
    pMouse->Release();
    CheckDXErrorFatal( pKeyboard->Unacquire());
    pKeyboard->Release();
    pInput->Release();
};

int ruIsKeyDown( int key )
{
    return g_keys[ key ];
};

int ruIsKeyHit( int key )
{
    return g_keysHit[ key ];
};

int	ruIsKeyUp	( int key )
{
    return g_keysUp[ key ];
}

int ruIsMouseDown( int button )
{
    if( (int)button >= 4 )
        return 0;

    return mouseData.rgbButtons[ button ];
}

int ruIsMouseHit( int button )
{
    return g_mouseHit[ button ];
}

int ruGetMouseX( )
{
    return mouseX;
}

int ruGetMouseY( )
{
    return mouseY;
}

int ruGetMouseWheel( )
{
    return mouseWheel;
}

int ruGetMouseXSpeed( )
{
    return mouseData.lX;
}

int ruGetMouseYSpeed( )
{
    return mouseData.lY;
}

int ruGetMouseWheelSpeed( )
{
    return mouseData.lZ;
}
