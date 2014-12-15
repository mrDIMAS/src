#pragma once

#include "Game.h"

class NumericSlider
{
private:
	float mValue;
	float mfMinimum, mfMaximum;
	float mfStep;
	ruButtonHandle mGUIIncreaseButton;
	ruButtonHandle mGUIDecreaseButton;
	ruTextHandle mGUIText;
	ruTextHandle mGUIValueText;
public:
	explicit NumericSlider( float x, float y, float minimum, float maximum, float step,  ruTextureHandle buttonImage, const char * text );
	void Update(  );
	float GetValue();
	void SetValue( float value );
	~NumericSlider();
	void SetVisible( bool state );
};