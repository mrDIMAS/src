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

class GUIText : public GUINode {
protected:
    BitmapFont * mFont;
    string mText;
    int mTextAlign;
    RECT mRect;
public:
    static vector<GUIText*> msTextList;

    explicit GUIText( const string & theText, float theX, float theY, float theWidth, float theHeight, ruVector3 theColor, int theAlpha, int theTextAlign, BitmapFont * theFont );
    virtual ~GUIText( );
    RECT GetBoundingRect();
    int GetTextAlignment();
    string & GetText();
    BitmapFont * GetFont();
    void SetText( const string & text );
};
