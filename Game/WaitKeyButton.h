#pragma once

#include "Game.h"
#include "GUI.h"
#include <Windows.h>

class WaitKeyButton
{
private:
    bool mGrabKey;
	int mSelectedKey;
    const char * mDesc;
public:    
	explicit WaitKeyButton();
	int GetSelectedKey();
    void SetSelected( int i );
    void Draw( float x, float y, ruTextureHandle buttonImage, const char * text );
};