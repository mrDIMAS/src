#include "NumericSlider.h"
#include "GUI.h"



void NumericSlider::Draw( float x, float y, TextureHandle buttonImage, const char * text ) {
    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    y = y + 1.5f * buttonHeight;
    GUIState increase = DrawGUIButton( x + captionWidth + 2.5f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", gui->font, Vector3( 0, 255, 0 ), 1 );
    GUIState decrease = DrawGUIButton( x + captionWidth, y, buttonWidth, buttonHeight, buttonImage, "<", gui->font, Vector3( 0, 255, 0 ), 1 );
    DrawGUIText( Format( "%.1f", value ).c_str(), x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 32, 16, gui->font, Vector3( 0, 255, 0 ), 1 );
    DrawGUIText( text, x, y + textHeight / 2, captionWidth, textHeight, gui->font, Vector3( 0, 255, 0 ), 0 );

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
