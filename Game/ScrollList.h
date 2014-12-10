#pragma once

#include "Game.h"

class ScrollList
{
private:
    int mCurrentValue;
    vector<string> mValues;
public:
    explicit ScrollList();
	void SetCurrentValue( int value );
    int GetCurrentValue();
    void AddValue( string val );
    void Draw( float x, float y, ruTextureHandle buttonImage, const char * text );
};