#include "NumericSlider.h"
#include "GUI.h"



void NumericSlider::Draw( float x, float y )
{
    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    y = y + 1.5f * buttonHeight;
	ruSetGUINodePosition( mGUIIncreaseButton, x + captionWidth + 4.5f * buttonWidth, y );
	ruSetGUINodePosition( mGUIDecreaseButton, x + captionWidth + buttonWidth, y );
    ruSetGUINodePosition( mGUIText, x, y + textHeight / 2 );
	ruSetGUINodeText( mGUIValueText, Format( "%.1f", mValue ).c_str() );
	ruSetGUINodePosition( mGUIValueText, x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2 );

    if( ruIsButtonHit( mGUIIncreaseButton )  ) {
        if( mValue < mfMaximum )
            mValue += mfStep;
    }
    if( ruIsButtonHit( mGUIDecreaseButton ) ) {
        if( mValue > mfMinimum )
            mValue -= mfStep;
    }
}

NumericSlider::NumericSlider( float minimum, float maximum, float step, ruTextureHandle buttonImage, const char * text )
{
	float buttonWidth = 32;
	float buttonHeight = 32;

	int textHeight = 16;
	int captionWidth = 100;

    mValue = maximum;

    mfMinimum = minimum;
    mfMaximum = maximum;

    mfStep = step;

	mGUIText = ruCreateGUIText( text, 0, 0, captionWidth, textHeight, pGUI->mFont, ruVector3( 255, 255, 255 ), 0 );
	mGUIValueText = ruCreateGUIText( "Value", 0, 0, 3.15f * buttonWidth, 16, pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
	mGUIIncreaseButton = ruCreateGUIButton( 0, 0, buttonWidth, buttonHeight, buttonImage, ">", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
	mGUIDecreaseButton = ruCreateGUIButton( 0, 0, buttonWidth, buttonHeight, buttonImage, "<", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
}

void NumericSlider::SetValue( float value )
{
	mValue = value;
}

float NumericSlider::GetValue()
{
	return mValue;
}
