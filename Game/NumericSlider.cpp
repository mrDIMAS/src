#include "NumericSlider.h"
#include "GUI.h"



void NumericSlider::Draw( float x, float y, ruTextureHandle buttonImage, const char * text ) {
    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    y = y + 1.5f * buttonHeight;
    ruGUIState increase = ruDrawGUIButton( x + captionWidth + 2.5f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", gui->font, ruVector3( 0, 255, 0 ), 1 );
    ruGUIState decrease = ruDrawGUIButton( x + captionWidth, y, buttonWidth, buttonHeight, buttonImage, "<", gui->font, ruVector3( 0, 255, 0 ), 1 );
    ruDrawGUIText( Format( "%.1f", value ).c_str(), x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 32, 16, gui->font, ruVector3( 0, 255, 0 ), 1 );
    ruDrawGUIText( text, x, y + textHeight / 2, captionWidth, textHeight, gui->font, ruVector3( 0, 255, 0 ), 0 );

    if( increase.mouseLeftClicked ) {
        if( value < fMaximum ) {
            value += fStep;
        }
    }
    if( decrease.mouseLeftClicked ) {
        if( value > fMinimum ) {
            value -= fStep;
        }
    }
}

NumericSlider::NumericSlider( float minimum, float maximum, float step ) {
    value = maximum;

    fMinimum = minimum;
    fMaximum = maximum;

    fStep = step;
}
