#include "Precompiled.h"

#include "WaitKeyButton.h"
#include "Utils.h"

vector<WaitKeyButton*> WaitKeyButton::msWaitKeyList;

void WaitKeyButton::Update( ) {
	if( mGUIButton->IsHit() ) {
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
			mGUIButton->GetText()->SetText( "[ Key ]" );
		} else {
			mGUIButton->GetText()->SetText( "[Key]" );
		}
		if( mAnimCounter > 20 ) {
			mAnimCounter = 0;
		}
		mAnimCounter++;
	} else {
		mGUIButton->GetText()->SetText( GetKeyName( mSelectedKey ));
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

WaitKeyButton::WaitKeyButton( float x, float y, shared_ptr<ruTexture> buttonImage, const string & text ) {
    int textHeight = 16;
    float buttonWidth = 60;
    float buttonHeight = 32;
    mDesc = " ";
	mAnimCounter = 0;
    mGrabKey = false;
    mGUIButton = ruButton::Create( x, y, buttonWidth, buttonHeight, buttonImage, "[Key]", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center );
    mGUIText = ruText::Create( text, x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left );
	WaitKeyButton::msWaitKeyList.push_back( this );
}

int WaitKeyButton::GetSelectedKey() {
    return mSelectedKey;
}

void WaitKeyButton::SetVisible( bool state ) {
    mGUIText->SetVisible( state );
    mGUIButton->SetVisible( state );
}

void WaitKeyButton::AttachTo( ruGUINode * node ) {
	mGUIText->Attach( node );
	mGUIButton->Attach( node );
}

WaitKeyButton::~WaitKeyButton() {
	msWaitKeyList.erase( find( msWaitKeyList.begin(), msWaitKeyList.end(), this ));
}

