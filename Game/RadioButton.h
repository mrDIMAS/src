#pragma once

#include "Game.h"

class RadioButton
{
private:
	bool mOn;
	ruTextHandle mGUIText;
	ruButtonHandle mGUIButton;
public:
    explicit RadioButton( ruTextureHandle buttonImage, const char * text );
    void Draw( float x, float y );
	bool IsEnabled( );
	void SetEnabled( bool state );
	void SetVisible( bool state )
	{
		ruSetGUINodeVisible( mGUIText, state );
		ruSetGUINodeVisible( mGUIButton, state );
	}
};