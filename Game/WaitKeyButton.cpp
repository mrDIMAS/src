#include "Precompiled.h"

#include "WaitKeyButton.h"
#include "Utils.h"

vector<WaitKeyButton*> WaitKeyButton::msWaitKeyList;

void WaitKeyButton::Update( ) {
	if( ruIsButtonHit( mGUIButton ) ) {
		// skip "grab key" mode from other buttons
		for( auto pWaitKey : WaitKeyButton::msWaitKeyList ) {
			pWaitKey->mGrabKey = false;
		}
		mGrabKey = true;
	}

	if( mGrabKey ) {
		for( int i = KEY_Esc + 1; i < 255; i++ ) {
			if( ruIsKeyDown( i )) {
				SetSelected( i );
				mGrabKey = false;
			}
		}
		if( ruIsKeyHit( KEY_Esc )) {
			mGrabKey = false;
		}

		if( mAnimCounter < 10 ) {
			ruSetGUINodeText( ruGetButtonText( mGUIButton ), "[ Key ]" );
		} else {
			ruSetGUINodeText( ruGetButtonText( mGUIButton ), "[Key]" );
		}
		if( mAnimCounter > 20 ) {
			mAnimCounter = 0;
		}
		mAnimCounter++;
	} else {
		ruSetGUINodeText( ruGetButtonText( mGUIButton ), GetKeyName( mSelectedKey ) );
	}
}

void WaitKeyButton::SetSelected( int i ) {
    mDesc = GetKeyName( i );
	int lastKey = mSelectedKey;
    mSelectedKey = i;
	// swap keys if duplicate found
	for( auto pWaitKey : WaitKeyButton::msWaitKeyList ) {
		if( pWaitKey != this ) {
			if( pWaitKey->mSelectedKey == mSelectedKey ) {
				pWaitKey->mSelectedKey = lastKey;
			}
		}
	}
}

WaitKeyButton::WaitKeyButton( float x, float y, ruTextureHandle buttonImage, const string & text ) {
    int textHeight = 16;
    float buttonWidth = 60;
    float buttonHeight = 32;
    mDesc = " ";
	mAnimCounter = 0;
    mGrabKey = false;
    mGUIButton = ruCreateGUIButton( x, y, buttonWidth, buttonHeight, buttonImage, "[Key]", pGUI->mFont, ruVector3( 255, 255, 255 ), 1 );
    mGUIText = ruCreateGUIText( text, x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, pGUI->mFont, ruVector3( 255, 255, 255 ), 0 );
	WaitKeyButton::msWaitKeyList.push_back( this );
}

int WaitKeyButton::GetSelectedKey() {
    return mSelectedKey;
}

void WaitKeyButton::SetVisible( bool state ) {
    ruSetGUINodeVisible( mGUIText, state );
    ruSetGUINodeVisible( mGUIButton, state );
}

void WaitKeyButton::AttachTo( ruGUINodeHandle node ) {
	ruAttachGUINode( mGUIText, node );
	ruAttachGUINode( mGUIButton, node );
}

WaitKeyButton::~WaitKeyButton() {
	msWaitKeyList.erase( find( msWaitKeyList.begin(), msWaitKeyList.end(), this ));
}

