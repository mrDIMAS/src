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

#include "Vertex.h"

Vertex::Vertex() {

}

Vertex::Vertex( float x, float y, float z, float tx, float ty ) {
    mPosition.x = x;
    mPosition.y = y;
    mPosition.z = z;

    mTexCoord.x = tx;
    mTexCoord.y = ty;
}

Vertex::Vertex( ruVector3 & theCoords, ruVector3 & theNormals, ruVector2 & theTexCoords, ruVector3 & theTangents ) {
    mPosition = theCoords;
    mNormal = theNormals;
    mTexCoord = theTexCoords;
    mTangent = theTangents;
}


Vertex2D::Vertex2D( float x, float y, float z, float tx, float ty, int color /*= 0 */ ) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->tx = tx;
    this->ty = ty;
    this->color = color;
}
