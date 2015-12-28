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

class Vertex {
public:
    ruVector3 mPosition;
    ruVector3 mNormal;
    ruVector2 mTexCoord;
    ruVector3 mTangent;

    Vertex( ruVector3 & theCoords, ruVector3 & theNormals, ruVector2 & theTexCoords, ruVector3 & theTangents );
    Vertex( float x, float y, float z, float tx, float ty );
    Vertex( );
};

class VertexSkin {
public:
	ruVector3 mPosition;
	ruVector3 mNormal;
	ruVector2 mTexCoord;
	ruVector3 mTangent;
	ruVector4 mBoneIndices;
	ruVector4 mBoneWeights;

	VertexSkin( const ruVector3 & position, const ruVector3 & normal, const ruVector2 & texCoord, const ruVector3 & tangent, const ruVector4 & boneIndices, const ruVector4 & boneWeights  );;
	VertexSkin( const Vertex & v, const ruVector4 & boneIndices, const ruVector4 & boneWeights );
	VertexSkin( );
};

class Vertex2D {
public:
    float x, y, z;
    float tx, ty;
    int color;
    Vertex2D( float x, float y, float z, float tx, float ty, int color = 0 );
    Vertex2D( ) {
        x = y = z = tx = ty = 0;
        color = 0xFFFFFFFF;
    }
};