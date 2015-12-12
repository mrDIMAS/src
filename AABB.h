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


#include "Vertex.h"

class AABB {
public:
	ruVector3 mFrustumMax;
	ruVector3 mFrustumMin;
    ruVector3 mMin;
    ruVector3 mMax;
    ruVector3 mCenter;
    explicit AABB( );
    explicit AABB( const ruVector3 & min, const ruVector3 & max );
    explicit AABB( const vector< Vertex > & vertices );
    bool IsPointInside( const ruVector3 & v );
    bool IsTriangleInside( const ruVector3 & v1, const ruVector3 & v2, const ruVector3 & v3 );
};