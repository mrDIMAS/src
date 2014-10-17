#include "RadioButton.h"
#include "GUI.h"

void RadioButton::Draw( float x, float y, TextureHandle buttonImage, const char * text ) {
    int textHeight = 16;
    float buttonWidth = 110;
    float buttonHeight = 32;

    GUIState fxaaControlButton = DrawGUIButton( x, y, buttonWidth, buttonHeight, buttonImage, text, gui->font, Vector3( 0, 255, 0 ), 1 );
    DrawGUIText( on ? "ÂÊËÞ×ÅÍÎ" : "ÂÛÊËÞ×ÅÍÎ", x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, gui->font, on ? Vector3( 0, 255, 0 ) : Vector3( 255, 0, 0 ), 0 );

    if( fxaaControlButton.mouseLeftClicked )
        on = !on;
}

RadioButton::RadioButton() {
    on = false;
}
