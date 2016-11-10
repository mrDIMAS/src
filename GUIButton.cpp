/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
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
#include "GUIScene.h"

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
						mEventList[ ruGUIAction::OnClick ]();
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

GUIButton::GUIButton(const weak_ptr<GUIScene> & scene, int x, int y, int w, int h, shared_ptr<Texture> texture, const string & text,  const shared_ptr<BitmapFont> & font, ruVector3 color, ruTextAlignment textAlign, int alpha )
    : GUIRect( scene, x, y, w, h, texture, color, alpha ) {
    mpText = dynamic_pointer_cast<GUIText>( mScene.lock()->CreateText( text, 0, 0, w, h, font, color, textAlign, alpha ));
	mpText->SetLayer(0xFF);
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

shared_ptr<ruText> GUIButton::GetText() {
    return mpText;
}

GUIButton::~GUIButton() {

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
