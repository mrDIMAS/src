#include "List.h"
#include "GUI.h"

void List::Draw( float x, float y, ruTextureHandle buttonImage, const char * text ) {
    float buttonWidth = 32;
    float buttonHeight = 32;

    int textHeight = 16;
    int captionWidth = 100;

    y = y + 1.5f * buttonHeight;
    ruGUIState increase = ruDrawGUIButton( x + captionWidth + 4.5f * buttonWidth, y, buttonWidth, buttonHeight, buttonImage, ">", gui->font, ruVector3( 0, 255, 0 ), 1 );
    ruGUIState decrease = ruDrawGUIButton( x + captionWidth, y, buttonWidth, buttonHeight, buttonImage, "<", gui->font, ruVector3( 0, 255, 0 ), 1 );

    ruDrawGUIText( text, x, y + textHeight / 2, captionWidth, textHeight, gui->font, ruVector3( 0, 255, 0 ), 0 );

    if( values.size() ) {
        ruDrawGUIText( values[ currentValue ].c_str(), x + captionWidth + buttonWidth * 1.25f, y  + textHeight / 2, 3.15f * buttonWidth, 16, gui->font, ruVector3( 0, 255, 0 ), 1 );

        if( increase.mouseLeftClicked )
            if( currentValue < values.size() - 1 ) {
                currentValue++;
            }

        if( decrease.mouseLeftClicked )
            if( currentValue > 0 ) {
                currentValue--;
            }
    }
}

void List::AddValue( string val ) {
    values.push_back( val );
}

int List::GetCurrentValue() {
    return currentValue;
}

List::List() {
    currentValue = 0;
}
