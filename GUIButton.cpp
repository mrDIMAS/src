/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#include "Precompiled.h"
#include "Texture.h"
#include "GUIButton.h"

vector< GUIButton* > GUIButton::msButtonList;

void GUIButton::Update() {
	if( IsVisible() ) {
		CalculateTransform();

		int mouseX = ruInput::GetMouseX();
		int mouseY = ruInput::GetMouseY();

		mPicked = false;
		mLeftPressed = false;
		mRightPressed = false;
		mLeftHit = false;
		mRightHit = false;

		if( mActive ) {
			mpText->SetColor( ruVector3( 255, 255, 255 ));

			if( mouseX > mGlobalX && mouseX < ( mGlobalX + mWidth ) && mouseY > mGlobalY && mouseY < ( mGlobalY + mHeight ) ) {
				mPicked = true;

				if( ruInput::IsMouseHit( ruInput::MouseButton::Left )) {
					if( IsGotAction( ruGUIAction::OnClick )) {
						mEventList[ ruGUIAction::OnClick ].DoActions();
					}

					mLeftHit = true;
				}

				if( ruInput::IsMouseHit( ruInput::MouseButton::Right )) {
					mRightHit = true;
				}

				if( ruInput::IsMouseDown( ruInput::MouseButton::Left )) {
					mLeftPressed = true;
				}

				if( ruInput::IsMouseDown( ruInput::MouseButton::Right )) {
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

GUIButton::GUIButton( int x, int y, int w, int h, shared_ptr<Texture> texture, const string & text, BitmapFont * font, ruVector3 color, ruTextAlignment textAlign, int alpha )
    : GUIRect( x, y, w, h, texture, color, alpha, true ) {
    mpText = new GUIText( text, 0, 0, w, h, color, alpha, textAlign, font );
	mpText->Attach( this );
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

bool GUIButton::IsPressed() {
    return mLeftPressed;
}

bool GUIButton::IsPicked() {
    return mPicked;
}


bool GUIButton::IsHit() const {
    return mLeftHit;
}

bool GUIButton::IsRightHit() const {
    return mRightHit;
}

bool GUIButton::IsRightPressed() {
    return mRightPressed;
}

ruText * GUIButton::GetText() {
    return mpText;
}

GUIButton::~GUIButton() {
	delete mpText;
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

ruButton * ruButton::Create( int x, int y, int w, int h, shared_ptr<ruTexture> texture, const string & text, ruFont * font, ruVector3 color, ruTextAlignment textAlign, int alpha  ) {
	return new GUIButton( x, y, w, h, std::dynamic_pointer_cast<Texture>( texture ), text, dynamic_cast<BitmapFont*>( font ), color, textAlign, alpha );
}