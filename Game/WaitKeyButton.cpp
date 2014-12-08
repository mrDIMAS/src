#include "WaitKeyButton.h"
#include "Utils.h"



void WaitKeyButton::Draw( float x, float y, ruTextureHandle buttonImage, const char * text ) {
    int textHeight = 16;
    float buttonWidth = 60;
    float buttonHeight = 32;

    ruGUIState controlButton = ruDrawGUIButton( x, y, buttonWidth, buttonHeight, buttonImage, grabKey ? "[ Key ]" : desc.c_str(), gui->font, ruVector3( 0, 255, 0 ), 1 );
    ruDrawGUIText( text, x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, gui->font, ruVector3( 0, 255, 0 ), 0 );

    if( controlButton.mouseLeftClicked ) {
        grabKey = true;
    }

    if( grabKey ) {
        for( int i = 0; i < 255; i++ ) {
            if( ruIsKeyDown( i )) {
                SetSelected( i );
                grabKey = false;
            }
        }
    }
}

void WaitKeyButton::SetSelected( int i ) {
    desc = GetKeyName( i );

    selectedKey = i;
}

WaitKeyButton::WaitKeyButton() {
    desc = " ";
    grabKey= false;
}

