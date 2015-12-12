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

class Mesh : public RendererComponent {
public:
    class Triangle {
    public:
        unsigned short mA;
        unsigned short mB;
        unsigned short mC;

        Triangle( unsigned short vA, unsigned short vB, unsigned short vC );
    };

	class Bone {
	public:
		int mMatrixID; // number of this bone in list of bones of a mesh
		SceneNode * mNode;
		D3DXMATRIX mMatrix;
		Bone( SceneNode * node, int matrixID );
		Bone( );
	};

	class BoneProxy {
	public:
		float mWeight;
		int mID; // id of the scene node represents bone in scene
		Bone * mRealBone;
	};

	class BoneGroup {
	public:
		BoneProxy mBone[ 4 ];
		int mBoneCount;
	};

public:
    IDirect3DVertexBuffer9 * mVertexBuffer;
    IDirect3DIndexBuffer9 * mIndexBuffer;
    Texture * mDiffuseTexture;
    Texture * mNormalTexture;
	Texture * mHeightTexture; // for parallax mapping

    vector<Vertex> mVertices;
    vector<Triangle> mTriangles;
    vector<BoneGroup> mWeightTable;

	vector<Bone*> mBones;
    // mesh can be shared between multiple nodes
	vector<SceneNode*> mOwnerList;
    AABB mAABB;
    Octree * mOctree;
	bool mSkinned;
    float mOpacity;
	static IDirect3DVertexDeclaration9 * msVertexDeclaration;
	static IDirect3DVertexDeclaration9 * msVertexDeclarationSkin;
public:
	Bone * AddBone( SceneNode * node );
	vector<Bone*> & GetBones();
	void OnResetDevice();
	void OnLostDevice();
    static unordered_map< IDirect3DTexture9*, vector< Mesh*>> msMeshList;
    static void Register( Mesh * mesh );
	static void EraseOrphanMeshes();
	static void CleanUp();
    explicit Mesh();
    virtual ~Mesh();
	void LinkTo( SceneNode * owner );
	void Unlink( SceneNode * owner );
    vector<SceneNode*> & GetOwners();
    void CreateVertexBuffer();
    void CreateIndexBuffer( vector< Triangle > & idc );
    void CreateHardwareBuffers();
    Texture * GetDiffuseTexture();
    Texture * GetNormalTexture();
    void Render();
	void RenderEx( IDirect3DIndexBuffer9 * ib, int faceCount );
};