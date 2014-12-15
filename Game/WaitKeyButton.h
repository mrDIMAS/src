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
	ruTextHandle mGUIText;
	ruButtonHandle mGUIButton;
public:    
	explicit WaitKeyButton( ruTextureHandle buttonImage, const char * text );
	int GetSelectedKey();
    void SetSelected( int i );
    void Draw( float x, float y );
	void SetVisible( bool state )
	{
		ruSetGUINodeVisible( mGUIText, state );
		ruSetGUINodeVisible( mGUIButton, state );
	}
};