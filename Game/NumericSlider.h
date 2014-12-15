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
	explicit NumericSlider( float minimum, float maximum, float step,  ruTextureHandle buttonImage, const char * text );
	void Draw( float x, float y );
	float GetValue();
	void SetValue( float value );
	~NumericSlider()
	{	
		ruFreeGUINode( mGUIValueText );
		ruFreeGUINode( mGUIText );
		ruFreeGUINode( mGUIIncreaseButton );
		ruFreeGUINode( mGUIDecreaseButton );
	}
	void SetVisible( bool state )
	{
		ruSetGUINodeVisible( mGUIText, state );
		ruSetGUINodeVisible( mGUIValueText, state );
		ruSetGUINodeVisible( mGUIIncreaseButton, state );
		ruSetGUINodeVisible( mGUIDecreaseButton, state );
	}
};