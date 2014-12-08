#pragma once

#include "Game.h"

class List {
private:
    int currentValue;
    vector<string> values;
public:
    List();
    int GetCurrentValue();
    void AddValue( string val );
    void Draw( float x, float y, ruTextureHandle buttonImage, const char * text );
};