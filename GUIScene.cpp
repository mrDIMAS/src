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
#include "GUIScene.h"
#include "Texture.h"

bool GUIScene::IsVisible( ) const {
	return mVisible;
}

void GUIScene::SetVisible( bool visible ) {
	mVisible = visible;
}

void GUIScene::SetOpacity( float opacity ) {
	mOpacity = opacity;
	if ( mOpacity < 0.0f ) {
		mOpacity = 0.0f;
	}
	if ( mOpacity > 1.0f ) {
		mOpacity = 1.0f;
	}
}

float GUIScene::GetOpacity( ) const {
	return mOpacity;
}

void GUIScene::SortNodes( ) {
	sort( mNodeList.begin( ), mNodeList.end( ), LayerSorter );
}

Engine * const GUIScene::GetEngine() const {
	return mEngine;
}

bool GUIScene::LayerSorter( const shared_ptr<GUINode>& node1, const shared_ptr<GUINode>& node2 ) {
	return node1->GetLayer( ) < node2->GetLayer( );
}

GUIScene::GUIScene(Engine * engine) :
	mEngine(engine),
	mVisible( true ), 
	mOpacity( 1.0f ) {

}

GUIScene::~GUIScene( ) {

}



vector<shared_ptr<GUINode>>& GUIScene::GetNodeList( ) {
	return mNodeList;
}

shared_ptr<IGUINode> GUIScene::CreateNode( ) {
	shared_ptr<GUINode> node( new GUINode( shared_from_this( ) ) );
	mNodeList.push_back( node );
	SortNodes( );
	return std::move( node );
}

vector<shared_ptr<GUIText>>& GUIScene::GetTextList( ) {
	return mTextList;
}

shared_ptr<IText> GUIScene::CreateText( const string & theText, float theX, float theY, float theWidth, float theHeight, const shared_ptr<IFont>& theFont, Vector3 theColor, TextAlignment theTextAlign, int theAlpha ) {
	shared_ptr<GUIText> text( new GUIText( shared_from_this( ), theText, theX, theY, theWidth, theHeight, theColor, theAlpha, theTextAlign, dynamic_pointer_cast<BitmapFont>( theFont ) ) );
	mNodeList.push_back( text );
	mTextList.push_back( text );
	SortNodes( );
	return text;
}

shared_ptr<IRect> GUIScene::CreateRect( float theX, float theY, float theWidth, float theHeight, const shared_ptr<ITexture>& theTexture, Vector3 theColor, int theAlpha ) {
	shared_ptr<GUIRect> rect( new GUIRect( shared_from_this( ), theX, theY, theWidth, theHeight, dynamic_pointer_cast<Texture>( theTexture ), theColor, theAlpha ) );
	mNodeList.push_back( rect );
	mRectList.push_back( rect );
	SortNodes( );
	return rect;
}

vector<shared_ptr<GUIButton>>& GUIScene::GetButtonList( ) {
	return mButtonList;
}

vector<shared_ptr<GUIRect>>& GUIScene::GetRectList( ) {
	return mRectList;
}

shared_ptr<IButton> GUIScene::CreateButton( int x, int y, int w, int h, const shared_ptr<ITexture>& texture, const string & text, const shared_ptr<IFont>& font, Vector3 color, TextAlignment textAlign, int alpha ) {
	shared_ptr<GUIButton> button( new GUIButton( shared_from_this( ), x, y, w, h, dynamic_pointer_cast<Texture>( texture ), text, dynamic_pointer_cast<BitmapFont>( font ), color, textAlign, alpha ) );
	button->GetText( )->Attach( button );
	mNodeList.push_back( button );
	mRectList.push_back( button );
	mButtonList.push_back( button );
	SortNodes( );
	return button;
}
