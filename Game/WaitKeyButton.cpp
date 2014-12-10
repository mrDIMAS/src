#include "WaitKeyButton.h"
#include "Utils.h"



void WaitKeyButton::Draw( float x, float y, ruTextureHandle buttonImage, const char * text )
{
    int textHeight = 16;
    float buttonWidth = 60;
    float buttonHeight = 32;

    ruGUIState controlButton = ruDrawGUIButton( x, y, buttonWidth, buttonHeight, buttonImage, mGrabKey ? "[ Key ]" : mDesc, pGUI->mFont, ruVector3( 0, 255, 0 ), 1 );
    ruDrawGUIText( text, x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, pGUI->mFont, ruVector3( 0, 255, 0 ), 0 );

    if( controlButton.mouseLeftClicked )
        mGrabKey = true;

    if( mGrabKey ) {
        for( int i = 0; i < 255; i++ ) {
            if( ruIsKeyDown( i )) {
                SetSelected( i );
                mGrabKey = false;
            }
        }
    }
}

void WaitKeyButton::SetSelected( int i )
{
    mDesc = GetKeyName( i );
    mSelectedKey = i;
}

WaitKeyButton::WaitKeyButton()
{
    mDesc = " ";
    mGrabKey= false;
}

int WaitKeyButton::GetSelectedKey()
{
	return mSelectedKey;
}

