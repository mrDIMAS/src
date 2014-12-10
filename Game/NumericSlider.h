#pragma once

#include "Game.h"

class NumericSlider
{
private:
	float mValue;
	float mfMinimum, mfMaximum;
	float mfStep;
public:
	explicit NumericSlider( float minimum, float maximum, float step );
	void Draw( float x, float y, ruTextureHandle buttonImage, const char * text );
	float GetValue();
	void SetValue( float value );
};