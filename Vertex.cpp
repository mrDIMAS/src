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

#include "Vertex.h"

Vertex::Vertex( ) {

}

Vertex::Vertex( const Vertex & v, const ruVector4 & boneIndices, const ruVector4 & boneWeights ) :
	mPosition( v.mPosition ),
	mNormal( v.mNormal ),
	mTexCoord( v.mTexCoord ),
	mTangent( v.mTangent ),
	mBoneIndices( boneIndices ),
	mBoneWeights( boneWeights ) {

}

Vertex::Vertex( const ruVector3 & position, const ruVector3 & normal, const ruVector2 & texCoord, const ruVector3 & tangent, const ruVector4 & boneIndices, const ruVector4 & boneWeights ) :
	mPosition( position ),
	mNormal( normal ),
	mTexCoord( texCoord ),
	mTangent( tangent ),
	mBoneIndices( boneIndices ),
	mBoneWeights( boneWeights ) {

}

Vertex::Vertex( const ruVector3 & position, const ruVector2 & texCoord, const ruVector4 & color ) :
	mPosition( position ),
	mTexCoord( texCoord ),
	mBoneIndices( color ) {

}

Vertex::Vertex( const ruVector3 & position, const ruVector2 & texCoord ) :
	mPosition( position ),
	mTexCoord( texCoord ) {

}

Triangle::Triangle( unsigned short vA, unsigned short vB, unsigned short vC ) {
	mA = vA;
	mB = vB;
	mC = vC;
}