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

#pragma once

class GUINode;
class GUIButton;
class GUIText;
class GUIRect;
class BitmapFont;
class Texture;

class GUIFactory {
private:
	static vector<weak_ptr<GUINode>> msNodeList;
	static vector<weak_ptr<GUIText>> msTextList;
	static vector<weak_ptr<GUIRect>> msRectList;
	static vector<weak_ptr<GUIButton>> msButtonList;
	template<typename Type>
	static void RemoveUnreferenced( vector<weak_ptr<Type>> & objList );
public:
	static vector<weak_ptr<GUINode>> & GetNodeList();
	static vector<weak_ptr<GUIText>> & GetTextList();
	static vector<weak_ptr<GUIRect>> & GetRectList();
	static vector<weak_ptr<GUIButton>> & GetButtonList();
	static shared_ptr<GUINode> CreateNode();
	static shared_ptr<GUIText> CreateText( const string & theText, float theX, float theY, float theWidth, float theHeight, ruVector3 theColor, int theAlpha, ruTextAlignment theTextAlign, const shared_ptr<BitmapFont> & theFont );
	static shared_ptr<GUIRect> CreateRect( float theX, float theY, float theWidth, float theHeight, const shared_ptr<Texture> & theTexture, ruVector3 theColor, int theAlpha );
	static shared_ptr<GUIButton> CreateButton( int x, int y, int w, int h, const shared_ptr<Texture> & texture, const string & text, const shared_ptr<BitmapFont> & font, ruVector3 color, ruTextAlignment textAlign, int alpha );;
};
