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
#include "GUINode.h"
#include "GUIFactory.h"
#include "Engine.h"

void GUINode::SetAlpha( int alpha ) {
    mAlpha = alpha;
	if( mControlChildAlpha ) {
		for( auto & pChild : mChildList ) {
			pChild->SetAlpha( alpha );
		}
	}
    PackColor();
}

void GUINode::SetColor( ruVector3 color ) {
    mColor = color;
    PackColor();
}

void GUINode::PackColor() {
    mColorPacked = D3DCOLOR_ARGB( mAlpha, (int)mColor.x, (int)mColor.y, (int)mColor.z );
}

GUINode::GUINode() :     
	mX( 0 ),
	mY( 0 ),
	mWidth( 0 ),
	mHeight( 0 ),
	mVisible( true ),
	mGlobalX( 0.0f ),
	mGlobalY( 0.0f ),
	mControlChildAlpha( false ),
	mLastMouseInside( false )
{
    SetColor( ruVector3( 255, 255, 255 ));
    SetAlpha( 255 );
}

GUINode::~GUINode() {

}

void GUINode::SetTexture( const shared_ptr<ruTexture> & pTexture ) {
    mpTexture = std::dynamic_pointer_cast<Texture>( pTexture );
}

shared_ptr<ruTexture> GUINode::GetTexture() {
    return mpTexture;
}

bool GUINode::IsVisible() {
	bool visibility = mVisible;
	if( mParent.use_count() ) {
		shared_ptr<GUINode> & pParent = mParent.lock();
		visibility &= pParent->IsVisible();
	}
    return visibility;
}

void GUINode::SetVisible( bool visible ) {
    mVisible = visible;
}

void GUINode::SetSize( float w, float h ) {
    mWidth = w * pEngine->GetGUIWidthScaleFactor();
    mHeight = h * pEngine->GetGUIHeightScaleFactor();
}

float GUINode::GetHeight() {
    return mHeight / pEngine->GetGUIHeightScaleFactor();
}

float GUINode::GetWidth() {
    return mWidth / pEngine->GetGUIWidthScaleFactor();
}

float GUINode::GetY() {
    return mY / pEngine->GetGUIHeightScaleFactor();
}

float GUINode::GetX() {
    return mX / pEngine->GetGUIWidthScaleFactor();
}

int GUINode::GetAlpha() {
    return mAlpha;
}

ruVector3 GUINode::GetColor() {
    return mColor;
}

ruVector2 GUINode::GetSize() {
    return ruVector2( mWidth / pEngine->GetGUIWidthScaleFactor(), mHeight / pEngine->GetGUIHeightScaleFactor() );
}

ruVector2 GUINode::GetPosition() {
    return ruVector2( GetX(), GetY() );
}

void GUINode::SetPosition( float x, float y ) {
    mX = x * pEngine->GetGUIWidthScaleFactor();
    mY = y * pEngine->GetGUIHeightScaleFactor();
	CalculateTransform();
}

int GUINode::GetPackedColor() {
    return mColorPacked;
}

void GUINode::Attach( const shared_ptr<ruGUINode> & parent )
{
	shared_ptr<GUINode> & parentNode = std::dynamic_pointer_cast<GUINode>( parent );
	parentNode->mChildList.push_back( shared_from_this() );
	mParent = parentNode;
}

void GUINode::CalculateTransform() {
	mGlobalX = 0.0f;
	mGlobalY = 0.0f;
	if( mParent.use_count() ) {
		shared_ptr<GUINode> & pParent = mParent.lock();
		pParent->CalculateTransform();
		mGlobalX = mX + pParent->mGlobalX;
		mGlobalY = mY + pParent->mGlobalY;
	} else {
		mGlobalX = mX;
		mGlobalY = mY;
	}
}

void GUINode::AddAction( ruGUIAction act, const ruDelegate & delegat ) {
	mEventList[ act ].AddListener( delegat );
}

bool GUINode::IsGotAction( ruGUIAction act ) {
	auto iter = mEventList.find( act );
	return iter != mEventList.end();
}

bool GUINode::IsMouseInside() {
	CalculateTransform();
	int mouseX = ruInput::GetMouseX();
	int mouseY = ruInput::GetMouseY();
	return mouseX > mGlobalX && mouseX < ( mGlobalX + mWidth ) && 
		   mouseY > mGlobalY && mouseY < ( mGlobalY + mHeight );
}

void GUINode::OnMouseEnter() {
	if( IsGotAction( ruGUIAction::OnMouseEnter )) {
		mEventList[ ruGUIAction::OnMouseEnter ].DoActions();
	}
}

void GUINode::OnMouseLeave() {
	if( IsGotAction( ruGUIAction::OnMouseLeave )) {
		mEventList[ ruGUIAction::OnMouseLeave ].DoActions();
	}
}

void GUINode::DoActions() {
	if( IsMouseInside() ) {
		if( ruInput::IsMouseHit( ruInput::MouseButton::Left )) {
			OnClick();
		}
		if( !mLastMouseInside ) {
			OnMouseEnter();
		} 
	} else {
		if( mLastMouseInside ) {
			OnMouseLeave();
		}
	}
}

void GUINode::OnClick() {
	if( IsGotAction( ruGUIAction::OnClick )) {
		mEventList[ ruGUIAction::OnClick ].DoActions();
	}
}

void GUINode::RemoveAllActions() {
	mEventList.clear();
}

void GUINode::RemoveAction( ruGUIAction act ) {
	mEventList.erase( act );
}

void GUINode::SetChildAlphaControl( bool control ) {
	mControlChildAlpha = control;
}

bool GUINode::GotParent()
{
	return mParent.use_count() > 0;
}

shared_ptr<GUINode> GUINode::GetParent()
{
	return mParent.lock();
}

ruGUINode::~ruGUINode() {

}