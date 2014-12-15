#include "WaitKeyButton.h"
#include "Utils.h"



void WaitKeyButton::Draw( float x, float y )
{
    int textHeight = 16;
    float buttonWidth = 60;
    float buttonHeight = 32;

	ruSetGUINodePosition( mGUIButton, x, y );
	ruSetGUINodeText( ruGetButtonText( mGUIButton ), mGrabKey ? "[ Key ]" : mDesc );
	ruSetGUINodePosition( mGUIText, x + buttonWidth * 1.1f, y + textHeight / 2 );

    if( ruIsButtonHit( mGUIButton ) )
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

WaitKeyButton::WaitKeyButton( ruTextureHandle buttonImage, const char * text )
{
	int textHeight = 16;
	float buttonWidth = 60;
	float buttonHeight = 32;

    mDesc = " ";
    mGrabKey= false;
	mGUIButton = ruCreateGUIButton( 0, 0, buttonWidth, buttonHeight, buttonImage, mGrabKey ? "[ Key ]" : mDesc, pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
	mGUIText = ruCreateGUIText( text, 0, 0, 100, textHeight, pGUI->mFont, ruVector3( 0, 255, 0 ), 0 );
}

int WaitKeyButton::GetSelectedKey()
{
	return mSelectedKey;
}

