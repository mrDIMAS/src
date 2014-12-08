#include "RadioButton.h"
#include "GUI.h"

void RadioButton::Draw( float x, float y, ruTextureHandle buttonImage, const char * text ) {
    int textHeight = 16;
    float buttonWidth = 110;
    float buttonHeight = 32;

    ruGUIState fxaaControlButton = ruDrawGUIButton( x, y, buttonWidth, buttonHeight, buttonImage, text, gui->font, ruVector3( 0, 255, 0 ), 1 );
    ruDrawGUIText( on ? "ÂÊËÞ×ÅÍÎ" : "ÂÛÊËÞ×ÅÍÎ", x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, gui->font, on ? ruVector3( 0, 255, 0 ) : ruVector3( 255, 0, 0 ), 0 );

    if( fxaaControlButton.mouseLeftClicked ) {
        on = !on;
    }
}

RadioButton::RadioButton() {
    on = false;
}
