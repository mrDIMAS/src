#pragma once

#include "Common.h"
#include "GUIRenderer.h"

class Cursor : public GUIRect
{
public:
	explicit Cursor( int w, int h, Texture * texture ) : GUIRect( 0, 0, w, h, texture, ruVector3( 255, 255, 255 ), 255, false )
	{

	}
    void Show();
    void Hide();
};