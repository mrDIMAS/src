#include "Precompiled.h"

#include "GUIButton.h"

vector< GUIButton* > GUIButton::msButtonList;

void GUIButton::Update() {
	if( IsVisible() ) {
		CalculateTransform();

		int mouseX = ruGetMouseX();
		int mouseY = ruGetMouseY();

		mPicked = false;
		mLeftPressed = false;
		mRightPressed = false;
		mLeftHit = false;
		mRightHit = false;

		if( mActive ) {
			mpText->SetColor( ruVector3( 255, 255, 255 ));

			if( mouseX > mGlobalX && mouseX < ( mGlobalX + mWidth ) && mouseY > mGlobalY && mouseY < ( mGlobalY + mHeight ) ) {
				mPicked = true;

				if( ruIsMouseHit( MB_Left )) {
					if( IsGotAction( ruGUIAction::OnClick )) {
						mEventList[ ruGUIAction::OnClick ].DoActions();
					}

					mLeftHit = true;
				}

				if( ruIsMouseHit( MB_Right )) {
					mRightHit = true;
				}

				if( ruIsMouseDown( MB_Left )) {
					mLeftPressed = true;
				}

				if( ruIsMouseDown( MB_Right )) {
					mRightPressed = true;
				}

				mpText->SetColor( ruVector3( 255, 0, 0 ));
			}

			SetColor( mInitColor );
		} else {
			SetColor( ruVector3( 90, 90, 90 ));
			mpText->SetColor( ruVector3( 90, 90, 90 ));
		}
	}
}

GUIButton::GUIButton( int x, int y, int w, int h, Texture* texture, const string & text, BitmapFont * font, ruVector3 color, int textAlign, int alpha )
    : GUIRect( x, y, w, h, texture, color, alpha, true ) {
    mpText = new GUIText( text, 0, 0, w, h, color, alpha, textAlign, font );
	mpText->AttachTo( this );
    msButtonList.push_back( this );
    mPicked = false;
    mLeftPressed = false;
    mRightPressed = false;
    mLeftHit = false;
    mRightHit = false;
	mActive = true;
	mInitColor = color;
    pickedColor = ruVector3( 255, 255, 255 );
}

void GUIButton::SetPickedColor( ruVector3 val ) {
    pickedColor = val;
}

ruVector3 GUIButton::GetPickedColor() const {
    return pickedColor;
}

bool GUIButton::IsLeftPressed() {
    return mLeftPressed;
}

bool GUIButton::IsPicked() {
    return mPicked;
}


bool GUIButton::IsLeftHit() const {
    return mLeftHit;
}

bool GUIButton::IsRightHit() const {
    return mRightHit;
}

bool GUIButton::IsRightPressed() {
    return mRightPressed;
}

GUIText * GUIButton::GetText() {
    return mpText;
}

GUIButton::~GUIButton() {
    msButtonList.erase( find( msButtonList.begin(), msButtonList.end(), this ));
}

void GUIButton::SetAlpha( int alpha )
{
	GUINode::SetAlpha( alpha );
	mpText->SetAlpha( alpha );
}

void GUIButton::SetActive( bool state )
{
	mActive = state;
}
