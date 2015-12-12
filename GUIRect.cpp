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

#include "GUIRect.h"
#include "GUIRenderer.h"

vector<GUIRect*> GUIRect::msRectList;

GUIRect::~GUIRect() {
	auto iter = find( msRectList.begin(), msRectList.end(), this );
	if( iter != msRectList.end() ) {
		msRectList.erase( iter );
	}
}

GUIRect::GUIRect() {

}

GUIRect::GUIRect( float theX, float theY, float theWidth, float theHeight, Texture * theTexture, ruVector3 theColor, int theAlpha, bool selfRegister ) {
    mX = theX;
    mY = theY;
    mWidth = theWidth;
    mHeight = theHeight;
    mpTexture = theTexture;
    mVisible = true;
    SetColor( theColor );
    SetAlpha( theAlpha );
    if( selfRegister ) {
        msRectList.push_back( this );
    }
}

void GUIRect::GetSixVertices( Vertex2D * vertices )
{
	vertices[ 0 ] = Vertex2D( mGlobalX, mGlobalY, 0, 0, 0, mColorPacked );
	vertices[ 1 ] = Vertex2D( mGlobalX + mWidth, mGlobalY, 0, 1, 0, mColorPacked );
	vertices[ 2 ] = Vertex2D( mGlobalX, mGlobalY + mHeight, 0, 0, 1, mColorPacked );
	vertices[ 3 ] = Vertex2D( mGlobalX + mWidth, mGlobalY, 0, 1, 0, mColorPacked );
	vertices[ 4 ] = Vertex2D( mGlobalX + mWidth, mGlobalY + mHeight, 0, 1, 1, mColorPacked );
	vertices[ 5 ] = Vertex2D( mGlobalX, mGlobalY + mHeight, 0, 0, 1, mColorPacked );
}
