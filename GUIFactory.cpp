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
#include "GUIFactory.h"
#include "GUIButton.h"
#include "Texture.h"
#include "BitmapFont.h"

vector<weak_ptr<GUINode>> GUIFactory::msNodeList;
vector<weak_ptr<GUIText>> GUIFactory::msTextList;
vector<weak_ptr<GUIRect>> GUIFactory::msRectList;
vector<weak_ptr<GUIButton>> GUIFactory::msButtonList;

vector<weak_ptr<GUINode>> & GUIFactory::GetNodeList() {
	RemoveUnreferenced( msNodeList );
	return msNodeList;
}

template<typename Type>
void GUIFactory::RemoveUnreferenced( vector<weak_ptr<Type>> & objList ) {
	for( auto iter = objList.begin(); iter != objList.end(); ) {		
		if((*iter).use_count()) {
			++iter;
		} else {
			iter = objList.erase( iter );
		}
	}
}

shared_ptr<GUINode> GUIFactory::CreateNode() {
	shared_ptr<GUINode> node( new GUINode );
	msNodeList.push_back( node );
	return std::move( node );
}

vector<weak_ptr<GUIText>> & GUIFactory::GetTextList() {
	RemoveUnreferenced( msTextList );
	return msTextList;
}

shared_ptr<GUIText> GUIFactory::CreateText( const string & theText, float theX, float theY, float theWidth, float theHeight, ruVector3 theColor, int theAlpha, ruTextAlignment theTextAlign, const shared_ptr<BitmapFont> & theFont )
{
	shared_ptr<GUIText> text( new GUIText( theText, theX, theY, theWidth, theHeight, theColor, theAlpha, theTextAlign, theFont ));
	msNodeList.push_back( text );
	msTextList.push_back( text );
	return text;
}

shared_ptr<GUIRect> GUIFactory::CreateRect( float theX, float theY, float theWidth, float theHeight, const shared_ptr<Texture> & theTexture, ruVector3 theColor, int theAlpha )
{
	shared_ptr<GUIRect> rect( new GUIRect( theX, theY, theWidth, theHeight, theTexture, theColor, theAlpha ));
	msNodeList.push_back( rect );
	msRectList.push_back( rect );
	return rect;
}

vector<weak_ptr<GUIButton>> & GUIFactory::GetButtonList() {
	RemoveUnreferenced( msButtonList );
	return msButtonList;
}

vector<weak_ptr<GUIRect>> & GUIFactory::GetRectList() {
	RemoveUnreferenced( msRectList );
	return msRectList;
}

shared_ptr<GUIButton> GUIFactory::CreateButton( int x, int y, int w, int h, const shared_ptr<Texture> & texture, const string & text, const shared_ptr<BitmapFont> & font, ruVector3 color, ruTextAlignment textAlign, int alpha )
{
	shared_ptr<GUIButton> button( new GUIButton( x, y, w, h, texture, text, font, color, textAlign, alpha ));
	button->GetText()->Attach( button );	
	msNodeList.push_back( button );
	msRectList.push_back( button );
	msButtonList.push_back( button );
	return button;
}
