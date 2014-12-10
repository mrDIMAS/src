#include "RadioButton.h"
#include "GUI.h"
#include "Menu.h"

void RadioButton::Draw( float x, float y, ruTextureHandle buttonImage, const char * text )
{
    int textHeight = 16;
    float buttonWidth = 110;
    float buttonHeight = 32;

    ruGUIState fxaaControlButton = ruDrawGUIButton( x, y, buttonWidth, buttonHeight, buttonImage, text, pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
    ruDrawGUIText( mOn ? pMainMenu->mLocalization.GetString( "toggleEnabled" ) : pMainMenu->mLocalization.GetString( "toggleDisabled" ), x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, pGUI->mFont, mOn ? ruVector3( 0, 255, 0 ) : ruVector3( 255, 0, 0 ), 0 );

    if( fxaaControlButton.mouseLeftClicked )
        mOn = !mOn;
}

RadioButton::RadioButton()
{
    mOn = false;
}

void RadioButton::SetEnabled( bool state )
{
	mOn = state;
}

bool RadioButton::IsEnabled()
{
	return mOn;
}
