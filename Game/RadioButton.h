#pragma once

#include "Game.h"

class RadioButton {
public:

    bool on;

    RadioButton();

    void Draw( float x, float y, ruTextureHandle buttonImage, const char * text );
};