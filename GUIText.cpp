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

#include "GUIText.h"

vector<GUIText*> GUIText::msTextList;

GUIText::GUIText( const string & theText, float theX, float theY, float theWidth,
                  float theHeight, ruVector3 theColor, int theAlpha, int theTextAlign, BitmapFont * theFont ) {
    mX = theX;
    mY = theY;
    mWidth = theWidth;
    mHeight = theHeight;
    mText = theText;
    mFont = theFont;
    mTextAlign = theTextAlign;
    SetColor( theColor );
    SetAlpha( theAlpha );
    mVisible = true;
    msTextList.push_back( this );
}

GUIText::~GUIText() {
    msTextList.erase( find( msTextList.begin(), msTextList.end(), this ));
}

RECT GUIText::GetBoundingRect() {
	CalculateTransform();
    mRect.left = mGlobalX;
    mRect.top = mGlobalY;
    mRect.right = mGlobalX + mWidth;
    mRect.bottom = mGlobalY + mHeight;
    return mRect;
}

BitmapFont * GUIText::GetFont() {
    return mFont;
}

std::string & GUIText::GetText() {
    return mText;
}

int GUIText::GetTextAlignment() {
    return mTextAlign;
}

void GUIText::SetText( const string & text )
{
	mText = text;
}
