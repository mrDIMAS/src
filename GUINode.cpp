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
#include "GUINode.h"
#include "Renderer.h"
#include "GUIScene.h"
#include "Engine.h"

void GUINode::SetAlpha( int alpha ) {
	mAlpha = alpha;
	if ( mAlpha > 255 ) {
		mAlpha = 255;
	}
	if ( mAlpha < 0.0f ) {
		mAlpha = 0.0f;
	}
}

void GUINode::SetColor( ruVector3 color ) {
	mColor = color / 255.0f;
}

GUINode::GUINode( const weak_ptr<GUIScene> & scene ) :
	mScene( scene ),
	mX( 0 ),
	mY( 0 ),
	mWidth( 0 ),
	mHeight( 0 ),
	mVisible( true ),
	mGlobalX( 0.0f ),
	mGlobalY( 0.0f ),
	mLastMouseInside( false ),
	mLayer( 0 ) {
	SetColor( ruVector3( 255, 255, 255 ) );
	SetAlpha( 255 );
}

GUINode::~GUINode( ) {

}

void GUINode::SetTexture( const shared_ptr<ruTexture> & pTexture ) {
	mpTexture = std::dynamic_pointer_cast<Texture>( pTexture );
}

shared_ptr<ruTexture> GUINode::GetTexture( ) {
	return mpTexture;
}

bool GUINode::IsVisible( ) const {
	bool visibility = mVisible;
	if ( mParent.use_count( ) ) {
		shared_ptr<GUINode> & pParent = mParent.lock( );
		visibility &= pParent->IsVisible( );
	}
	return visibility;
}

void GUINode::SetVisible( bool visible ) {
	mVisible = visible;
}

void GUINode::SetSize( float w, float h ) {
	mWidth = w * mScene.lock()->GetEngine()->GetRenderer()->GetGUIWidthScaleFactor( );
	mHeight = h * mScene.lock()->GetEngine()->GetRenderer()->GetGUIHeightScaleFactor( );
}

float GUINode::GetHeight( ) {
	return mHeight / mScene.lock()->GetEngine()->GetRenderer()->GetGUIHeightScaleFactor( );
}

float GUINode::GetWidth( ) {
	return mWidth / mScene.lock()->GetEngine()->GetRenderer()->GetGUIWidthScaleFactor( );
}

float GUINode::GetY( ) {
	return mY / mScene.lock()->GetEngine()->GetRenderer()->GetGUIHeightScaleFactor( );
}

float GUINode::GetX( ) {
	return mX / mScene.lock()->GetEngine()->GetRenderer()->GetGUIWidthScaleFactor( );
}

int GUINode::GetAlpha( ) {
	return mAlpha;
}

ruVector3 GUINode::GetColor( ) {
	return mColor;
}

ruVector2 GUINode::GetSize( ) {
	return ruVector2( mWidth / mScene.lock()->GetEngine()->GetRenderer()->GetGUIWidthScaleFactor( ), mHeight / mScene.lock()->GetEngine()->GetRenderer()->GetGUIHeightScaleFactor( ) );
}

ruVector2 GUINode::GetPosition( ) {
	return ruVector2( GetX( ), GetY( ) );
}

ruVector2 GUINode::GetGlobalPosition( ) {
	CalculateTransform( );
	return ruVector2( mGlobalX, mGlobalY );
}

void GUINode::SetIndependentAlpha( bool useIndependent ) {
	mIndependentAlpha = useIndependent;
}

bool GUINode::IsIndependentAlpha( ) const {
	return mIndependentAlpha;
}

void GUINode::SetPosition( float x, float y ) {
	mX = x * mScene.lock()->GetEngine()->GetRenderer()->GetGUIWidthScaleFactor( );
	mY = y * mScene.lock()->GetEngine()->GetRenderer()->GetGUIHeightScaleFactor( );
	CalculateTransform( );
}

void GUINode::Attach( const shared_ptr<ruGUINode> & parent ) {
	shared_ptr<GUINode> & parentNode = std::dynamic_pointer_cast<GUINode>( parent );
	parentNode->mChildList.push_back( shared_from_this( ) );
	mParent = parentNode;
}

void GUINode::CalculateTransform( ) {
	mGlobalX = 0.0f;
	mGlobalY = 0.0f;
	if ( mParent.use_count( ) ) {
		shared_ptr<GUINode> & pParent = mParent.lock( );
		pParent->CalculateTransform( );
		mGlobalX = mX + pParent->mGlobalX;
		mGlobalY = mY + pParent->mGlobalY;
	} else {
		mGlobalX = mX;
		mGlobalY = mY;
	}
}

void GUINode::AddAction( ruGUIAction act, const ruDelegate & delegat ) {
	mEventList[ act ] += delegat;
}

bool GUINode::IsGotAction( ruGUIAction act ) {
	auto iter = mEventList.find( act );
	return iter != mEventList.end( );
}

bool GUINode::IsMouseInside( ) {
	CalculateTransform( );
	auto input = mScene.lock()->GetEngine()->GetInput();
	int mouseX = input->GetMouseX( );
	int mouseY = input->GetMouseY( );
	return mouseX > mGlobalX && mouseX < ( mGlobalX + mWidth ) &&
		mouseY > mGlobalY && mouseY < ( mGlobalY + mHeight );
}

void GUINode::OnMouseEnter( ) {
	if ( IsGotAction( ruGUIAction::OnMouseEnter ) ) {
		mEventList[ ruGUIAction::OnMouseEnter ]( );
	}
}

void GUINode::OnMouseLeave( ) {
	if ( IsGotAction( ruGUIAction::OnMouseLeave ) ) {
		mEventList[ ruGUIAction::OnMouseLeave ]( );
	}
}

void GUINode::DoActions( ) {
	if ( IsMouseInside( ) ) {
		if ( mScene.lock()->GetEngine()->GetInput()->IsMouseHit( ruInput::MouseButton::Left ) ) {
			OnClick( );
		}
		if ( !mLastMouseInside ) {
			OnMouseEnter( );
		}
	} else {
		if ( mLastMouseInside ) {
			OnMouseLeave( );
		}
	}
}

void GUINode::OnClick( ) {
	if ( IsGotAction( ruGUIAction::OnClick ) ) {
		mEventList[ ruGUIAction::OnClick ]( );
	}
}

void GUINode::RemoveAllActions( ) {
	mEventList.clear( );
}

weak_ptr<class ruGUIScene> GUINode::GetScene( ) {
	return mScene;
}

void GUINode::SetLayer( int layer ) {
	mLayer = layer;
	if ( !mScene.expired( ) ) { // so intrusive
		mScene.lock( )->SortNodes( );
	}
}

int GUINode::GetLayer( ) const {
	return mLayer;
}

void GUINode::Move( const ruVector2 & speed ) {
	float dx = speed.x * mScene.lock()->GetEngine()->GetRenderer()->GetGUIWidthScaleFactor( );
	float dy = speed.y * mScene.lock()->GetEngine()->GetRenderer()->GetGUIHeightScaleFactor( );

	mX += dx;
	mY += dy;
}

void GUINode::RemoveAction( ruGUIAction act ) {
	mEventList.erase( act );
}

bool GUINode::GotParent( ) {
	return mParent.use_count( ) > 0;
}

shared_ptr<GUINode> GUINode::GetParent( ) {
	return mParent.lock( );
}

ruGUINode::~ruGUINode( ) {

}