#pragma once

#include "Game.h"
#include "GUI.h"
#include <Windows.h>

class WaitKeyButton {
private:
    bool grabKey;
    string desc;
public:
    int selectedKey;

    WaitKeyButton();
    void SetSelected( int i );
    void Draw( float x, float y, TextureHandle buttonImage, const char * text );
};