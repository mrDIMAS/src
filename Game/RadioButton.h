#pragma once

#include "Game.h"

class RadioButton
{
private:
	bool mOn;
public:
    explicit RadioButton();
    void Draw( float x, float y, ruTextureHandle buttonImage, const char * text );
	bool IsEnabled( );
	void SetEnabled( bool state );
};