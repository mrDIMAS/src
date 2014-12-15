#pragma once

#include "Game.h"

class ScrollList
{
private:
    int mCurrentValue;
    vector<string> mValues;
	ruButtonHandle mGUIIncreaseButton;
	ruButtonHandle mGUIDecreaseButton;
	ruTextHandle mGUIText;
	ruTextHandle mGUIValueText;
public:
    explicit ScrollList( ruTextureHandle buttonImage, const char * text );
	virtual ~ScrollList();
	void SetCurrentValue( int value );
    int GetCurrentValue();
    void AddValue( string val );
    void Draw( float x, float y );
	void SetVisible( bool state )
	{
		ruSetGUINodeVisible( mGUIText, state );
		ruSetGUINodeVisible( mGUIValueText, state );
		ruSetGUINodeVisible( mGUIIncreaseButton, state );
		ruSetGUINodeVisible( mGUIDecreaseButton, state );
	}
};