/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#include "Precompiled.h"
#include "GUIButton.h"
#include "GUIScene.h"

LPDIRECTINPUT8 pInput;
LPDIRECTINPUTDEVICE8 pKeyboard;
LPDIRECTINPUTDEVICE8 pMouse;
DIMOUSESTATE mouseData;
HWND hwnd;

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

const char * keyNames [] = {
	"Esc",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"0",
	"Minus",
	"Equals",
	"Backspace",
	"Tab",
	"Q",
	"W",
	"E",
	"R",
	"T",
	"Y",
	"U",
	"I",
	"O",
	"P",
	"LBracket",
	"RBracket",
	"Enter",
	"LControl",
	"A",
	"S",
	"D",
	"F",
	"G",
	"H",
	"J",
	"K",
	"L",
	"Semicolon",
	"Apostrophe",
	"Grave",
	"LShift",
	"BackSlash",
	"Z",
	"X",
	"C",
	"V",
	"B",
	"N",
	"M",
	"Comma",
	"Period",
	"Slash",
	"RShift",
	"Multiply",
	"LAlt",
	"Space",
	"Capital",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"NumLock",
	"Scroll",
	"NumPad7",
	"NumPad8",
	"NumPad9",
	"SUBTRACT",
	"NUMPAD4",
	"NUMPAD5",
	"NUMPAD6",
	"ADD",
	"NUMPAD1",
	"NUMPAD2",
	"NUMPAD3",
	"NUMPAD0",
	"Decimal",
	"OEM_102",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"KANA",
	"ABNT_C1",
	"CONVERT",
	"NOCONVERT",
	"YEN",
	"ABNT_C2",
	"NUMPADEQUALS",
	"PREVTRACK",
	"AT",
	"COLON",
	"UNDERLINE",
	"KANJI",
	"STOP",
	"AX",
	"UNLABELED",
	"NEXTTRACK",
	"NUMPADENTER",
	"RCONTROL",
	"MUTE",
	"CALCULATOR",
	"PLAYPAUSE",
	"MEDIASTOP",
	"VOLUMEDOWN",
	"VOLUMEUP",
	"WEBHOME",
	"NUMPADCOMMA",
	"DIVIDE",
	"SYSRQ",
	"RMENU",
	"PAUSE",
	"HOME",
	"UP",
	"PRIOR",
	"LEFT",
	"RIGHT",
	"END",
	"DOWN",
	"NEXT",
	"INSERT",
	"DEL",
	"LWIN",
	"RWIN",
	"APPS",
	"POWER",
	"SLEEP",
	"WAKE",
	"WEBSEARCH",
	"WEBFAVORITES",
	"WEBREFRESH",
	"WEBSTOP",
	"WEBFORWARD",
	"WEBBACK",
	"MYCOMPUTER",
	"MAIL",
	"MEDIASELECT"
};

string ruInput::GetKeyName( ruInput::Key key ) {
	if( key >= ruInput::Key::Count ) {
		return "BadID";
	}
	return keyNames[ static_cast<int>( key ) - 1 ];
}

void ruInput::Update( ) {
    if( pKeyboard->GetDeviceState( sizeof( g_keys ), reinterpret_cast<void*>( &g_keys )) == DIERR_INPUTLOST ) {
        pKeyboard->Acquire();
    }

    if( pMouse->GetDeviceState( sizeof( DIMOUSESTATE ), reinterpret_cast<void*>( &mouseData )) == DIERR_INPUTLOST ) {
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

    if( mouseX < 0 ) {
        mouseX = 0;
    }
    if( mouseY < 0 ) {
        mouseY = 0;
    }
    if( mouseX > windowRect.right ) {
        mouseX = windowRect.right;
    }
    if( mouseY > windowRect.bottom ) {
        mouseY = windowRect.bottom;
    }

    memcpy( g_lastKeys, g_keys, sizeof( g_lastKeys ));
    memcpy( g_mousePressed, mouseData.rgbButtons, sizeof( g_mousePressed ));

	auto & guiSceneList = GUIScene::GetSceneList();
	for (auto weakScene : guiSceneList) {
		auto scene = weakScene.lock();
		auto & buttons = scene->GetButtonList();
		for (auto & pButton : buttons) {
			if (pButton->IsVisible()) {
				pButton->Update();
			}			
		}
	}
};

void ruInput::Init( HWND window ) {
    HINSTANCE hInstance = GetModuleHandle( 0 );
    hwnd = window;
    DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>( &pInput ),NULL);
    pInput->CreateDevice( GUID_SysKeyboard, &pKeyboard, NULL );
    pKeyboard->SetDataFormat( &c_dfDIKeyboard );
    pKeyboard->SetCooperativeLevel( hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE );
    pKeyboard->Acquire();
    pInput->CreateDevice( GUID_SysMouse, &pMouse, NULL );
    pMouse->SetDataFormat(&c_dfDIMouse);
    pMouse->SetCooperativeLevel( hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE );
    pMouse->Acquire();
    RECT initialWindowRect;
    GetWindowRect( hwnd, &initialWindowRect );
    GetClientRect( hwnd, &windowRect );
    AdjustWindowRect( &windowRect, GetWindowLong( hwnd, GWL_STYLE ), 0 );
    windowOffsetX = initialWindowRect.left - windowRect.left;
    windowOffsetY = initialWindowRect.top - windowRect.top ;
    GetClientRect( hwnd, &windowRect );
};

void ruInput::Destroy() {
    pMouse->Unacquire();
    pMouse->Release();
    pKeyboard->Unacquire();
    pKeyboard->Release();
    pInput->Release();
};

bool ruInput::IsKeyDown( Key key ) {
    return g_keys[ static_cast<int>( key ) ];
};

bool ruInput::IsKeyHit( Key key ) {
    return g_keysHit[ static_cast<int>( key ) ];
};

bool ruInput::IsKeyUp ( Key key ) {
	return g_keysUp[ static_cast<int>( key ) ];
}

bool ruInput::IsMouseDown( MouseButton button ) {
    if( (int)button >= 4 ) {
        return 0;
    }

    return mouseData.rgbButtons[ static_cast<int>( button ) ];
}

bool ruInput::IsMouseHit( MouseButton button ) {
    return g_mouseHit[ static_cast<int>( button )];
}

int ruInput::GetMouseX( ) {
    return mouseX;
}

int ruInput::GetMouseY( ) {
    return mouseY;
}

int ruInput::GetMouseWheel( ) {
    return mouseWheel;
}

int ruInput::GetMouseXSpeed( ) {
    return mouseData.lX;
}

int ruInput::GetMouseYSpeed( ) {
    return mouseData.lY;
}

int ruInput::GetMouseWheelSpeed( ) {
    return mouseData.lZ;
}

