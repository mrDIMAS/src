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

#include "Mesh.h"
#include "SceneNode.h"
#include "Engine.h"

typedef struct MeshDesc {
	IDirect3DIndexBuffer9 * mIB;
	int mFaceCount;
	MeshDesc() { };
	MeshDesc( IDirect3DIndexBuffer9 * indexBuffer, int faceCount ) : mIB( indexBuffer ), mFaceCount( faceCount ) {};
} MeshDesc;

class Room {
private:
	vector<Room*> mNeighbourList;
	unordered_map<Mesh*,MeshDesc> mIBList;
	ruVector3 mMin;
	ruVector3 mMax;
	bool Contains( const ruVector3 & point );
	bool Intersect( Room * room );
	static vector<Room*> msRoomList;
	Room( SceneNode * scene, const ruVector3 & theMin, const ruVector3 & theMax );
public:
	~Room();
	static void Scan( SceneNode * scene );
	// connect intersecting rooms
	static void Connect();
	// special rendering function
	void Render( );
};