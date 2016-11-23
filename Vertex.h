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

class Vertex {
public:
	Vector3 mPosition;
	Vector3 mNormal;
	Vector2 mTexCoord;
	Vector3 mTangent;
	Vector4 mBoneIndices; // Also used for passing color for GUI and Particle System
	Vector4 mBoneWeights;

	Vertex( const Vector3 & position, const Vector3 & normal, const Vector2 & texCoord, const Vector3 & tangent, const Vector4 & boneIndices, const Vector4 & boneWeights );;
	Vertex( const Vertex & v, const Vector4 & boneIndices, const Vector4 & boneWeights );
	Vertex( const Vector3 & position, const Vector2 & texCoord, const Vector4 & color );
	Vertex( const Vector3 & position, const Vector2 & texCoord );
	Vertex( );
};

class Triangle {
public:
	unsigned short mA;
	unsigned short mB;
	unsigned short mC;

	Triangle( ) : mA( 0 ), mB( 0 ), mC( 0 ) {
	}
	Triangle( unsigned short vA, unsigned short vB, unsigned short vC );
};