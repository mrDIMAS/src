#include "Precompiled.h"

#include "NumericSlider.h"
#include "GUI.h"

void Slider::Update( ) {
    ruSetGUINodeText( mGUIValueText, StringBuilder() << fixed << setprecision( 1 ) << mValue );

    if( ruIsButtonHit( mGUIIncreaseButton )  ) {
        if( mValue < mfMaximum ) {
            mValue += mfStep;
        }
    }
    if( ruIsButtonHit( mGUIDecreaseButton ) ) {
        if( mValue > mfMinimum ) {
            mValue -= mfStep;
        }
    }
}

Slider::Slider( float x, float y, float minimum, float maximum, float step, ruTextureHandle buttonImage, const string & text ) {
    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    mValue = maximum;

    mfMinimum = minimum;
    mfMaximum = maximum;

    mfStep = step;

    mGUIText = ruCreateGUIText( text, x, y + textHeight / 2, captionWidth, textHeight, pGUI->mFont, ruVector3( 255, 255, 255 ), 0 );
    mGUIValueText = ruCreateGUIText( "Value", x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 3.15f * buttonWidth, 16, pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
    mGUIIncreaseButton = ruCreateGUIButton( x + captionWidth + 4.0f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
    mGUIDecreaseButton = ruCreateGUIButton( x + captionWidth + buttonWidth, y, buttonWidth, buttonHeight, buttonImage, "<", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
}

void Slider::SetValue( float value ) {
    mValue = value;
}

float Slider::GetValue() {
    return mValue;
}

Slider::~Slider() {
    ruFreeGUINode( mGUIValueText );
    ruFreeGUINode( mGUIText );
    ruFreeGUINode( mGUIIncreaseButton );
    ruFreeGUINode( mGUIDecreaseButton );
}
