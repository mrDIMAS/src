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
#include "GUIRect.h"
#include "GUIScene.h"

GUIRect::~GUIRect() {

}

GUIRect::GUIRect() : GUINode(shared_ptr<GUIScene>(nullptr)) {

}

GUIRect::GUIRect(const weak_ptr<GUIScene> & scene, float theX, float theY, float theWidth, float theHeight, shared_ptr<Texture> theTexture, ruVector3 theColor, int theAlpha) :
	GUINode(scene) {
	SetPosition(theX, theY);
	SetSize(theWidth, theHeight);
	mpTexture = theTexture;
	mVisible = true;
	SetColor(theColor);
	SetAlpha(theAlpha);
}

void GUIRect::GetSixVertices(Vertex * vertices) {
	float alpha = (mScene.expired() || mIndependentAlpha) ? mAlpha / 255.0f : mScene.lock()->GetOpacity() * mAlpha / 255.0f;
	vertices[0] = Vertex(ruVector3(mGlobalX, mGlobalY, 0), ruVector2(0, 0), ruVector4(mColor, alpha));
	vertices[1] = Vertex(ruVector3(mGlobalX + mWidth, mGlobalY, 0), ruVector2(1, 0), ruVector4(mColor, alpha));
	vertices[2] = Vertex(ruVector3(mGlobalX, mGlobalY + mHeight, 0), ruVector2(0, 1), ruVector4(mColor, alpha));
	vertices[3] = Vertex(ruVector3(mGlobalX + mWidth, mGlobalY, 0), ruVector2(1, 0), ruVector4(mColor, alpha));
	vertices[4] = Vertex(ruVector3(mGlobalX + mWidth, mGlobalY + mHeight, 0), ruVector2(1, 1), ruVector4(mColor, alpha));
	vertices[5] = Vertex(ruVector3(mGlobalX, mGlobalY + mHeight, 0), ruVector2(0, 1), ruVector4(mColor, alpha));
}
