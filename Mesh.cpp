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
#include "Engine.h"
#include "Mesh.h"
#include "Octree.h"
#include "Texture.h"
#include "Vertex.h"
#include "ForwardRenderer.h"

IDirect3DVertexDeclaration9 * Mesh::msVertexDeclaration;
IDirect3DVertexDeclaration9 * Mesh::msVertexDeclarationSkin;

unordered_map< IDirect3DTexture9*, vector<weak_ptr<Mesh>>> Mesh::msMeshList;

Mesh::Mesh() : mHeightTexture( nullptr ), mDiffuseTexture( nullptr ),
			   mNormalTexture( nullptr ), mOctree( nullptr ), mOpacity( 1.0f ), mSkinned( false ) {
	if( !msVertexDeclaration ) {
		D3DVERTEXELEMENT9 vd[ ] = {
			{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
			D3DDECL_END()
		};

		Engine::I().GetDevice()->CreateVertexDeclaration( vd, &msVertexDeclaration );
	}
	if( !msVertexDeclarationSkin ) {
		D3DVERTEXELEMENT9 vd[ ] = {
			{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
			{ 0, 44, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 }, // bone indices
			{ 0, 60, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 }, // bone weights
			D3DDECL_END()
		};

		Engine::I().GetDevice()->CreateVertexDeclaration( vd, &msVertexDeclarationSkin );
	}
}

void Mesh::LinkTo( weak_ptr<SceneNode> owner ) {
	shared_ptr<SceneNode> pOwner = owner.lock();
	if( pOwner ) {
		mSkinned = pOwner->IsSkinned();
	}
	mOwnerList.push_back( owner );
}

void Mesh::Register( shared_ptr<Mesh> mesh ) {
	mesh->mAABB = AABB( mesh->mVertices );
    if( mesh->mOpacity > 0.99f ) { // pass it to deferred renderer
		if( mesh->mDiffuseTexture ) {
			auto textureGroup = Mesh::msMeshList.find( mesh->mDiffuseTexture->GetInterface() );

			if( textureGroup == Mesh::msMeshList.end()) {
				Mesh::msMeshList[ mesh->mDiffuseTexture->GetInterface() ] = vector<weak_ptr<Mesh>>();
			}

			Mesh::msMeshList[ mesh->mDiffuseTexture->GetInterface() ].push_back( mesh );
		}
    } else { // pass it to forward renderer
        Engine::I().GetForwardRenderer()->AddMesh( mesh );
    }
}

Mesh::~Mesh() {
    OnLostDevice();    
	for( auto pBone : mBones ) {
		delete pBone;
	}
}

void Mesh::CreateVertexBuffer() {
	int sizeBytes;
	void * data;
	vector<VertexSkin> skinVertices;

	if( mVertices.size() ) {		
		if( mSkinned ) {
			sizeBytes = mVertices.size() * sizeof( VertexSkin );
			for( int i = 0; i < mVertices.size(); i++ ) {
				Mesh::BoneGroup w = mBoneTable[i];
				ruVector4 boneIndices = ruVector4( -1, -1, -1, -1 );	
				ruVector4 boneWeights = ruVector4( 0.0f, 0.0f, 0.0f, 0.0f );
				for( int k = 0; k < w.mBoneCount; k++ ) {
					boneIndices.c[ k ] = w.mBone[k].mRealBone->mMatrixID;
					boneWeights.c[ k ] = w.mBone[k].mWeight;
				}
				skinVertices.push_back( VertexSkin( mVertices[i], boneIndices, boneWeights ));
			}
			data = &skinVertices[0];
		} else {
			sizeBytes = mVertices.size() * sizeof( Vertex );
			data = &mVertices[0];
		}
		

		// dont care about FVF, set it to simple D3DFVF_XYZ
		Engine::I().GetDevice()->CreateVertexBuffer( sizeBytes, D3DUSAGE_WRITEONLY, D3DFVF_XYZ, D3DPOOL_DEFAULT, &mVertexBuffer, 0 );
		
		void * vertexData = 0;
		mVertexBuffer->Lock( 0, 0, &vertexData, 0 );
		memcpy( vertexData, data, sizeBytes );
		mVertexBuffer->Unlock();
	}
}

Mesh::Bone * Mesh::AddBone( weak_ptr<SceneNode> node ) {
	Mesh::Bone * bone = nullptr;
	for( auto & pBone : mBones ) {
		if( pBone->mNode.lock() == node.lock() ) {
			bone = pBone;
			break;
		}
	}
	if( !bone ) {
		if( node.use_count()) {
			node.lock()->MakeBone();
		}
		bone = new Bone( node, mBones.size() );
		mBones.push_back( bone );
	}
	return bone;		
}

void Mesh::CreateIndexBuffer( vector< Triangle > & triangles ) {
	if( triangles.size() ) {
		int sizeBytes = triangles.size() * 3 * sizeof( unsigned short );
		Engine::I().GetDevice()->CreateIndexBuffer( sizeBytes,D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mIndexBuffer, 0 );		
		void * indexData = 0;
		mIndexBuffer->Lock( 0, 0, &indexData, 0 );
		memcpy( indexData, &triangles[ 0 ], sizeBytes );
		mIndexBuffer->Unlock();
	}
}

vector<weak_ptr<SceneNode>> & Mesh::GetOwners() {
    return mOwnerList;
}

void Mesh::CreateHardwareBuffers() {
    CreateVertexBuffer();
    CreateIndexBuffer( mTriangles );
}

void Mesh::CleanUp() {
	msVertexDeclaration->Release();
	msVertexDeclarationSkin->Release();
}

shared_ptr<Texture> & Mesh::GetDiffuseTexture() {
    return mDiffuseTexture;
}

shared_ptr<Texture> & Mesh::GetNormalTexture() {
    return mNormalTexture;
}

void Mesh::Render() {
	if( mSkinned ) {
		Engine::I().GetDevice()->SetStreamSource( 0, mVertexBuffer, 0, sizeof( VertexSkin ));
		Engine::I().GetDevice()->SetVertexDeclaration( msVertexDeclarationSkin );
	} else {
		Engine::I().GetDevice()->SetStreamSource( 0, mVertexBuffer, 0, sizeof( Vertex ));
		Engine::I().GetDevice()->SetVertexDeclaration( msVertexDeclaration );
	}
	
	if( mOctree ) {
		vector< Triangle > & id = mOctree->GetTrianglesToRender();
		if( id.size() ) {
			CreateIndexBuffer( id );
		}
	}
	Engine::I().GetDevice()->SetIndices( mIndexBuffer );
    if( mOctree ) {
#ifdef _OCTREE_DEBUG
        mOctree->VisualizeHierarchy();
        ruDrawGUIText( Format( "Nodes: %d, Triangles: %d", mOctree->mVisibleNodeCount, mOctree->mVisibleTriangleCount ).c_str(), 40, 40, 100, 50, g_font, ruVector3( 255, 0, 0 ), 1 );
#endif
        if( mOctree->mVisibleTriangleList.size() ) {
			Engine::I().DrawIndexedTriangleList( mVertices.size(), mOctree->mVisibleTriangleList.size() );
        }
    } else {
		Engine::I().DrawIndexedTriangleList( mVertices.size(), mTriangles.size() );
    }
}

void Mesh::RenderEx( IDirect3DIndexBuffer9 * ib, int faceCount ) {
	Engine::I().GetDevice()->SetIndices( ib );
	Engine::I().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, mVertices.size(), 0, faceCount );
	// each mesh renders in one DIP
	Engine::I().RegisterDIP();
}

void Mesh::OnLostDevice() {
		
}

void Mesh::OnResetDevice() {
	//CreateHardwareBuffers();
	//Mesh::Register( this );
}

vector<Mesh::Bone*> & Mesh::GetBones() {
	return mBones;
}

void Mesh::AddBoneGroup( const BoneGroup & bg ) {
	mBoneTable.push_back( bg );
}

bool Mesh::IsSkinned() const {
	return mSkinned;
}

vector<Mesh::Triangle> & Mesh::GetTriangles(){
	return mTriangles;
}

vector<Vertex> & Mesh::GetVertices() {
	return mVertices;
}

AABB Mesh::GetBoundingBox() const
{
	return mAABB;
}

bool Mesh::IsHardwareBuffersGood() {
	return mIndexBuffer && mVertexBuffer;
}

vector<Mesh::BoneGroup> & Mesh::GetBoneTable() {
	return mBoneTable;
}

void Mesh::AddVertex( const Vertex & vertex ) {
	mVertices.push_back( vertex );
}

float Mesh::GetOpacity() const {
	return mOpacity;
}

void Mesh::SetOpacity( float opacity ) {
	mOpacity = opacity;
}

void Mesh::AddTriangle( const Triangle & triangle ) {
	mTriangles.push_back( triangle );
}

void Mesh::SetHeightTexture( const shared_ptr<Texture> & heightTexture ) {
	mHeightTexture = heightTexture;
}

void Mesh::SetNormalTexture( const shared_ptr<Texture> & normalTexture ) {
	mNormalTexture = normalTexture;
}

void Mesh::SetDiffuseTexture( const shared_ptr<Texture>& diffuseTexture ) {
	mDiffuseTexture = diffuseTexture;
}

shared_ptr<Texture> & Mesh::GetHeightTexture(){
	return mHeightTexture;
}

Mesh::MeshMap & Mesh::GetMeshMap() {
	for( auto texGroupPairIter = msMeshList.begin(); texGroupPairIter != msMeshList.end();  ) {
		auto & texGroupPair = *texGroupPairIter;
		if( texGroupPair.second.size() ) {
			for( auto & meshIter = texGroupPair.second.begin(); meshIter != texGroupPair.second.end(); ) {
				if( (*meshIter).use_count() ) {
					++meshIter;
				} else {
					meshIter = texGroupPair.second.erase( meshIter );
				}
			}
			++texGroupPairIter;
		} else {
			texGroupPairIter = msMeshList.erase( texGroupPairIter );
		}
	}
	return msMeshList;
}

Mesh::Triangle::Triangle( unsigned short vA, unsigned short vB, unsigned short vC ) {
	mA = vA;
	mB = vB;
	mC = vC;
}

Mesh::Bone::Bone() : mMatrixID( 0 ) {
	D3DXMatrixIdentity( &mMatrix );
}

Mesh::Bone::Bone( weak_ptr<SceneNode> node, int matrixID ) : mNode( node ), mMatrixID( matrixID ) {
	D3DXMatrixIdentity( &mMatrix );
}
