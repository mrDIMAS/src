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

#pragma once

#include "GUINode.h"
#include "Vertex.h"

class Texture;

class GUIRect : public GUINode, public virtual IRect {
protected:
	friend class GUIScene;
	explicit GUIRect( const weak_ptr<GUIScene> & scene, float theX, float theY, float theWidth, float theHeight, shared_ptr<Texture> theTexture, Vector3 theColor, int theAlpha );
	explicit GUIRect( ); // special constructor for cursor
public:

	virtual ~GUIRect( );
	void GetSixVertices( Vertex * vertices );
};