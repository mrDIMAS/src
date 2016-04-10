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
#include "GUIRect.h"
#include "GUIRenderer.h"
#include "GUIFactory.h"

GUIRect::~GUIRect() {

}

GUIRect::GUIRect() {

}

GUIRect::GUIRect( float theX, float theY, float theWidth, float theHeight, shared_ptr<Texture> theTexture, ruVector3 theColor, int theAlpha ) {
	SetPosition( theX, theY );
	SetSize( theWidth, theHeight );
    mpTexture = theTexture;
    mVisible = true;
    SetColor( theColor );
    SetAlpha( theAlpha );
}

void GUIRect::GetSixVertices( Vertex2D * vertices ) {
	vertices[ 0 ] = Vertex2D( mGlobalX, mGlobalY, 0, 0, 0, mColorPacked );
	vertices[ 1 ] = Vertex2D( mGlobalX + mWidth, mGlobalY, 0, 1, 0, mColorPacked );
	vertices[ 2 ] = Vertex2D( mGlobalX, mGlobalY + mHeight, 0, 0, 1, mColorPacked );
	vertices[ 3 ] = Vertex2D( mGlobalX + mWidth, mGlobalY, 0, 1, 0, mColorPacked );
	vertices[ 4 ] = Vertex2D( mGlobalX + mWidth, mGlobalY + mHeight, 0, 1, 1, mColorPacked );
	vertices[ 5 ] = Vertex2D( mGlobalX, mGlobalY + mHeight, 0, 0, 1, mColorPacked );
}

shared_ptr<ruRect> ruRect::Create( float theX, float theY, float theWidth, float theHeight, const shared_ptr<ruTexture> & theTexture, ruVector3 theColor, int theAlpha ) {
	return GUIFactory::CreateRect( theX, theY, theWidth, theHeight, std::dynamic_pointer_cast<Texture>( theTexture ), theColor, theAlpha );
};