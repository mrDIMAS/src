#include "RadioButton.h"
#include "GUI.h"
#include "Menu.h"

void RadioButton::Draw( float x, float y )
{
    int textHeight = 16;
    float buttonWidth = 110;
    float buttonHeight = 32;

    ruSetGUINodePosition( mGUIButton, x, y );
    ruSetGUINodeText( mGUIText, mOn ? pMainMenu->GetLocalization()->GetString( "toggleEnabled" ) : pMainMenu->GetLocalization()->GetString( "toggleDisabled" ) );
	ruSetGUINodePosition( mGUIText, x + buttonWidth * 1.1f, y + textHeight / 2 );
    if( ruIsButtonHit( mGUIButton ))
	{
        mOn = !mOn;
	}
	ruSetGUINodeColor( mGUIText,  mOn ? ruVector3( 0, 255, 0 ) : ruVector3( 255, 0, 0 ) );
}

RadioButton::RadioButton( ruTextureHandle buttonImage, const char * text  )
{
    mOn = false;
	int textHeight = 16;
	float buttonWidth = 110;
	float buttonHeight = 32;
	mGUIText = ruCreateGUIText( "text", 0, 0, 100, textHeight, pGUI->mFont, ruVector3( 255, 0, 0 ), 0 );
	mGUIButton = ruCreateGUIButton( 0, 0, buttonWidth, buttonHeight, buttonImage, text, pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
}

void RadioButton::SetEnabled( bool state )
{
	mOn = state;
}

bool RadioButton::IsEnabled()
{
	return mOn;
}
