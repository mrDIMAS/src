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
#include "Renderer.h"
#include "Mesh.h"
#include "Texture.h"
#include "Vertex.h"
#include "SceneNode.h"

Mesh::Mesh() :
	mHeightTexture(nullptr),
	mDiffuseTexture(nullptr),
	mNormalTexture(nullptr),
	mOpacity(1.0f),
	mSkinned(false) {

}

Mesh::~Mesh() {

}

void Mesh::LinkTo(weak_ptr<SceneNode> owner) {
	shared_ptr<SceneNode> pOwner = owner.lock();
	if(pOwner) {
		mSkinned = pOwner->IsSkinned();
	}
	mOwnerList.push_back(owner);
}

void Mesh::CreateVertexBuffer() {
	int sizeBytes;
	void * data;
	vector<Vertex> skinVertices;

	if(mVertices.size()) {
		if(mSkinned) {
			sizeBytes = mVertices.size() * sizeof(Vertex);
			for(int i = 0; i < mVertices.size(); i++) {
				Mesh::BoneGroup w = mBoneTable[i];
				Vector4 boneIndices = Vector4(-1, -1, -1, -1);
				Vector4 boneWeights = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
				for(int k = 0; k < w.mBoneCount; k++) {
					boneIndices.c[k] = w.mBone[k].mRealBone->mMatrixID;
					boneWeights.c[k] = w.mBone[k].mWeight;
				}
				skinVertices.push_back(Vertex(mVertices[i], boneIndices, boneWeights));
			}
			data = &skinVertices[0];
		} else {
			sizeBytes = mVertices.size() * sizeof(Vertex);
			data = &mVertices[0];
		}

		pD3D->CreateVertexBuffer(sizeBytes, D3DUSAGE_WRITEONLY, D3DFVF_XYZ, D3DPOOL_DEFAULT, &mVertexBuffer, 0);

		void * vertexData = 0;
		mVertexBuffer->Lock(0, 0, &vertexData, 0);
		memcpy(vertexData, data, sizeBytes);
		mVertexBuffer->Unlock();
	}
}

shared_ptr<Mesh::Bone> Mesh::AddBone(weak_ptr<SceneNode> node) {
	shared_ptr<Bone> bone;
	for(auto & pBone : mBones) {
		if(pBone->mNode.lock() == node.lock()) {
			bone = pBone;
			break;
		}
	}
	if(!bone) {
		if(node.use_count()) {
			node.lock()->MakeBone();
		}
		bone = make_shared<Bone>(node, mBones.size());
		mBones.push_back(bone);
	}
	return bone;
}

void Mesh::CreateIndexBuffer(vector< Triangle > & triangles) {
	if(triangles.size()) {
		int sizeBytes = triangles.size() * sizeof(Triangle);
		pD3D->CreateIndexBuffer(sizeBytes, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mIndexBuffer, 0);
		void * indexData = 0;
		mIndexBuffer->Lock(0, 0, &indexData, 0);
		memcpy(indexData, &triangles[0], sizeBytes);
		mIndexBuffer->Unlock();
	}
}

vector<weak_ptr<SceneNode>> & Mesh::GetOwners() {
	// Erase pointers to nonexistent scene nodes
	for(auto ownerIter = mOwnerList.begin(); ownerIter != mOwnerList.end(); ) {
		if((*ownerIter).lock()) {
			++ownerIter;
		} else {
			ownerIter = mOwnerList.erase(ownerIter);
		}
	}

	return mOwnerList;
}

void Mesh::CreateHardwareBuffers() {
	CreateVertexBuffer();
	CreateIndexBuffer(mTriangles);
}

void Mesh::CalculateAABB() {
	mAABB = AABB(mVertices);
}

vector<shared_ptr<Mesh::Bone>> & Mesh::GetBones() {
	return mBones;
}

void Mesh::AddBoneGroup(const BoneGroup & bg) {
	mBoneTable.push_back(bg);
}

bool Mesh::IsSkinned() const {
	return mSkinned;
}

vector<Triangle> & Mesh::GetTriangles() {
	return mTriangles;
}

vector<Vertex> & Mesh::GetVertices() {
	return mVertices;
}

AABB Mesh::GetBoundingBox() const {
	return mAABB;
}

bool Mesh::IsHardwareBuffersGood() {
	return mIndexBuffer && mVertexBuffer;
}

vector<Mesh::BoneGroup> & Mesh::GetBoneTable() {
	return mBoneTable;
}

void Mesh::AddVertex(const Vertex & vertex) {
	mVertices.push_back(vertex);
}

float Mesh::GetOpacity() const {
	return mOpacity;
}

void Mesh::SetOpacity(float opacity) {
	mOpacity = opacity;
}

void Mesh::AddTriangle(const Triangle & triangle) {
	mTriangles.push_back(triangle);
}

Mesh::Bone::Bone() : mMatrixID(0) {

}

Mesh::Bone::Bone(weak_ptr<SceneNode> node, int matrixID) : mNode(node), mMatrixID(matrixID) {

}
