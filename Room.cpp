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
#include "Room.h"

vector<Room*> Room::msRoomList;

void Room::Render() {
	for( auto p : mIBList ) {
		p.first->RenderEx( p.second.mIB, p.second.mFaceCount );
	}
}

void Room::Connect() {
	for( auto pRoom : msRoomList ) {
		for( auto pOtherRoom : msRoomList ) {
			if( pRoom->Intersect( pOtherRoom )) {
				pRoom->mNeighbourList.push_back( pOtherRoom );
				pOtherRoom->mNeighbourList.push_back( pRoom );
			}
		}
	}
}

Room::Room( SceneNode * scene, const ruVector3 & theMin, const ruVector3 & theMax ) : mMax( theMax ), mMin( theMin ) {
	for( auto pSceneNode : scene->mChildList ) {
		// rooms named with '#' prefix
		if( pSceneNode->GetName().at( 0 ) != '#' ) {
			for( auto pMesh : pSceneNode->mMeshList ) {
				vector<Mesh::Triangle> triangleList;
				// static objects couldn't share meshes, so there is only one owner of the mesh
				SceneNode * mainOwner = pMesh->GetOwners().at( 0 );
				// do not process meshes with dynamic owners (i.e. scene node with non-static physical body)
				// but process static geometry, like level geometry with static physical body
				if( mainOwner->IsStatic() || !mainOwner->IsDynamic() ) {
					for( auto & triangle : pMesh->GetTriangles() ) {					
						ruVector3 a = pMesh->GetVertices()[ triangle.mA ].mPosition + mainOwner->GetPosition();
						ruVector3 b = pMesh->GetVertices()[ triangle.mB ].mPosition + mainOwner->GetPosition();
						ruVector3 c = pMesh->GetVertices()[ triangle.mC ].mPosition + mainOwner->GetPosition();
						if( Contains( a ) || Contains( b ) || Contains( c )) {
							triangleList.push_back( triangle );
						}
					}
					if( triangleList.size() ) {
						// create index buffer for mesh
						int sizeBytes = triangleList.size() * sizeof( Mesh::Triangle );
						IDirect3DIndexBuffer9 * ib = nullptr;
						Engine::I().GetDevice()->CreateIndexBuffer( sizeBytes,D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib, 0 );
						void * indexData = 0;
						ib->Lock( 0, 0, &indexData, 0 );
						memcpy( indexData, &triangleList[ 0 ], sizeBytes );
						ib->Unlock();
						// register this mesh as part of room
						mIBList[ pMesh ] = MeshDesc( ib, triangleList.size() );
					}
				}
			}
		}
	}	
}

void Room::Scan( SceneNode * scene ) {
	for( auto pSceneNode : scene->mChildList ) {
		if( pSceneNode->GetName().at( 0 ) == '#' ) {
			msRoomList.push_back( new Room( scene, pSceneNode->GetAABBMin(), pSceneNode->GetAABBMin() ));
		}
	}
}

bool Room::Intersect( Room * room ) {
	return Contains( room->mMax ) || Contains( room->mMin );
}

bool Room::Contains( const ruVector3 & point ) {
	return  point.x >= mMin.x && point.x <= mMax.x && 
		point.y >= mMin.y && point.y <= mMax.y &&
		point.z >= mMin.z && point.z <= mMax.z;
}

Room::~Room() {
	for( auto & p : mIBList ) {
		p.second.mIB->Release();
	}
	msRoomList.erase( find( msRoomList.begin(), msRoomList.end(), this ));
}
