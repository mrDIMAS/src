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

#include "GUINode.h"

class GUITextLine {
public:
	float mX, mY;
	string mSubstring;

	GUITextLine( float x, float y, const string & substring ) :
		mX( x ),
		mY( y ),
		mSubstring( substring )
	{
		
	}
};

class GUIText : public GUINode, public ruText {
protected:
	friend class GUIScene;
	explicit GUIText( const weak_ptr<GUIScene> & scene, const string & theText, float theX, float theY, float theWidth, float theHeight, ruVector3 theColor, int theAlpha, ruTextAlignment theTextAlign, const shared_ptr<BitmapFont> & theFont );
    shared_ptr<BitmapFont> mFont;
    string mText;
    ruTextAlignment mTextAlign;
	

	void BreakOnLines();
public:
	// Data for renderer
	vector<GUITextLine> mLines;
public:
    virtual ~GUIText( );
    ruTextAlignment GetTextAlignment();
    string & GetText();
    shared_ptr<BitmapFont> GetFont();


    virtual void SetText( const string & text );
};
