#include "WaitKeyButton.h"
#include "Utils.h"



void WaitKeyButton::Update( ) {
    ruSetGUINodeText( ruGetButtonText( mGUIButton ), mGrabKey ? "[ Key ]" : GetKeyName( mSelectedKey ) );

    if( ruIsButtonHit( mGUIButton ) ) {
        mGrabKey = true;
    }

    if( mGrabKey ) {
        for( int i = 0; i < 255; i++ ) {
            if( ruIsKeyDown( i )) {
                SetSelected( i );
                mGrabKey = false;
            }
        }
    }
}

void WaitKeyButton::SetSelected( int i ) {
    mDesc = GetKeyName( i );
    mSelectedKey = i;
}

WaitKeyButton::WaitKeyButton( float x, float y, ruTextureHandle buttonImage, const char * text ) {
    int textHeight = 16;
    float buttonWidth = 60;
    float buttonHeight = 32;
    mDesc = " ";
    mGrabKey = false;
    mGUIButton = ruCreateGUIButton( x, y, buttonWidth, buttonHeight, buttonImage, "[Key]", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
    mGUIText = ruCreateGUIText( text, x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, pGUI->mFont, ruVector3( 0, 255, 0 ), 0 );
}

int WaitKeyButton::GetSelectedKey() {
    return mSelectedKey;
}

void WaitKeyButton::SetVisible( bool state ) {
    ruSetGUINodeVisible( mGUIText, state );
    ruSetGUINodeVisible( mGUIButton, state );
}

