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