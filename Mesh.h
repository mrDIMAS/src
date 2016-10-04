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

#include "AABB.h"

class Octree;
class Texture;
class SceneNode;

class Mesh {
public:
	// Grim code ahead
	class Bone {
	public:
		int mMatrixID; // number of this bone in list of bones of a mesh
		weak_ptr<SceneNode> mNode;
		Bone( weak_ptr<SceneNode> node, int matrixID );
		Bone( );
	};

	class BoneProxy {
	public:
		float mWeight;
		int mID; // id of the scene node represents bone in scene
		shared_ptr<Bone> mRealBone;
	};

	class BoneGroup {
	public:
		BoneProxy mBone[ 4 ];
		int mBoneCount;
	};
private:
	vector<BoneGroup> mBoneTable;
	vector<shared_ptr<Bone>> mBones;
    // mesh can be shared between multiple nodes
	vector<weak_ptr<SceneNode>> mOwnerList;
    
	bool mSkinned;    
public:
	// Data for renderer
	AABB mAABB;
	vector<Vertex> mVertices;
	vector<Triangle> mTriangles;
	COMPtr<IDirect3DVertexBuffer9> mVertexBuffer;
	COMPtr<IDirect3DIndexBuffer9> mIndexBuffer;
	shared_ptr<Texture> mDiffuseTexture;
	shared_ptr<Texture> mNormalTexture;
	shared_ptr<Texture> mHeightTexture;
	
	float mOpacity;
public:
	shared_ptr<Bone> AddBone( weak_ptr<SceneNode> node );
	vector<shared_ptr<Bone>> & GetBones();
	void CalculateAABB();
    explicit Mesh();
    virtual ~Mesh();
	void LinkTo( weak_ptr<SceneNode> owner );
	// This method guaranteed to return list with only existing owners
    vector<weak_ptr<SceneNode>> & GetOwners();
    void CreateVertexBuffer();
    void CreateIndexBuffer( vector< Triangle > & idc );
    void CreateHardwareBuffers();
	AABB GetBoundingBox( ) const;
	vector<Vertex> & GetVertices();
	vector<Triangle> & GetTriangles();
	bool IsSkinned() const;
	void AddBoneGroup( const BoneGroup & bg );
	void AddTriangle( const Triangle & triangle );
	void SetOpacity( float opacity );
	float GetOpacity( ) const;
	void AddVertex( const Vertex & vertex );
	vector<BoneGroup> & GetBoneTable();
	bool IsHardwareBuffersGood();
};