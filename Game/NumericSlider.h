#pragma once

#include "Game.h"

class NumericSlider {
public:
    float value;
    float fMinimum, fMaximum;
    float fStep;

    NumericSlider( float minimum, float maximum, float step );

    void Draw( float x, float y, ruTextureHandle buttonImage, const char * text );
};