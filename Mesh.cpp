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

unordered_map< IDirect3DTexture9*, vector< Mesh*>> Mesh::msMeshList;

Mesh::Mesh() : mHeightTexture( nullptr ), mDiffuseTexture( nullptr ), mIndexBuffer( nullptr ), mVertexBuffer( nullptr ),
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

void Mesh::LinkTo( SceneNode * owner ) {
	mSkinned = owner->mIsSkinned;
	mOwnerList.push_back( owner );
}

void Mesh::Unlink( SceneNode * owner ) {
	auto iter = find( mOwnerList.begin(), mOwnerList.end(), owner ); 
	if( iter != mOwnerList.end() ) {
		mOwnerList.erase( iter );
	}
}

void Mesh::Register( Mesh * mesh ) {
	mesh->mAABB = AABB( mesh->mVertices );
    if( mesh->mOpacity > 0.99f ) { // pass it to deferred renderer
        auto textureGroup = Mesh::msMeshList.find( mesh->mDiffuseTexture->GetInterface() );

        if( textureGroup == Mesh::msMeshList.end()) {
            Mesh::msMeshList[ mesh->mDiffuseTexture->GetInterface() ] = vector< Mesh*>();
        }

        Mesh::msMeshList[ mesh->mDiffuseTexture->GetInterface() ].push_back( mesh );
    } else { // pass it to forward renderer
        Engine::I().GetForwardRenderer()->AddMesh( mesh );
    }
}

Mesh::~Mesh() {
    OnLostDevice();
    
	for( auto pBone : mBones ) {
		delete pBone;
	}

    if( mOctree ) {
        delete mOctree;
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

Mesh::Bone * Mesh::AddBone( SceneNode * node ) {
	Mesh::Bone * bone = nullptr;
	for( auto & pBone : mBones ) {
		if( pBone->mNode == node ) {
			bone = pBone;
			break;
		}
	}
	if( !bone ) {
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

vector<SceneNode*> & Mesh::GetOwners() {
    return mOwnerList;
}

void Mesh::CreateHardwareBuffers() {
    CreateVertexBuffer();
    CreateIndexBuffer( mTriangles );
}

void Mesh::EraseOrphanMeshes() {
	vector<Mesh*> eraseList;
	for( auto iMeshGroup : msMeshList ) {
		for( auto iMesh = iMeshGroup.second.begin(); iMesh != iMeshGroup.second.end(); ) {
			Mesh * pMesh = *iMesh;
			if( pMesh->GetOwners().size() == 0 ) {
				eraseList.push_back( pMesh );
				//delete pMesh;
				iMesh = iMeshGroup.second.erase( iMesh );
			} else {
				iMesh++;
			}
		}
	}
	for( auto pMesh : eraseList ) {
		delete pMesh;
	}
}

void Mesh::CleanUp() {
	vector<Mesh*> meshList;
	for( auto iMeshGroup : msMeshList ) {
		for( auto iMesh = iMeshGroup.second.begin(); iMesh != iMeshGroup.second.end(); iMesh++ ) {
			meshList.push_back( *iMesh );//delete (*iMesh);
		}
	}
	for( auto pMesh : meshList ) {
		delete pMesh;
	}
	msVertexDeclaration->Release();
	msVertexDeclarationSkin->Release();
}

shared_ptr<Texture> Mesh::GetDiffuseTexture() {
    return mDiffuseTexture;
}

shared_ptr<Texture> Mesh::GetNormalTexture() {
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
	bool removed = false;
	auto group = Mesh::msMeshList.find( mDiffuseTexture->GetInterface() );
	if( group != Mesh::msMeshList.end()) {
		auto & meshes = group->second;
		for( size_t i = 0; i < meshes.size(); i++ ) {
			if( meshes[i] == this ) {
				meshes.erase( meshes.begin() + i );
				removed = true;
			}
		}
		if( group->second.size() == 0 ) {
			Mesh::msMeshList.erase( group );
		}
	}

	if( !removed ) {
		Engine::I().GetForwardRenderer()->RemoveMesh( this );
	}
	if( mVertexBuffer ) {
		mVertexBuffer->Release();
	}
	if( mIndexBuffer ) {
		mIndexBuffer->Release();
	}
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;	

}

void Mesh::OnResetDevice() {
	CreateHardwareBuffers();
	Mesh::Register( this );
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

AABB Mesh::GetBoundingBox() {
	return mAABB;
}

bool Mesh::IsHardwareBuffersGood() {
	return mIndexBuffer && mVertexBuffer;
}

vector<BoneGroup> & Mesh::GetBoneTable() {
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

void Mesh::SetHeightTexture( shared_ptr<Texture> heightTexture ) {
	mHeightTexture = heightTexture;
}

void Mesh::SetNormalTexture( shared_ptr<Texture> normalTexture ) {
	mNormalTexture = normalTexture;
}

void Mesh::SetDiffuseTexture( shared_ptr<Texture> diffuseTexture ) {
	mDiffuseTexture = diffuseTexture;
}

shared_ptr<Texture> Mesh::GetHeightTexture(){
	return mHeightTexture;
}

Mesh::Triangle::Triangle( unsigned short vA, unsigned short vB, unsigned short vC ) {
	mA = vA;
	mB = vB;
	mC = vC;
}

Mesh::Bone::Bone() : mNode( nullptr ) {
	D3DXMatrixIdentity( &mMatrix );
}

Mesh::Bone::Bone( SceneNode * node, int matrixID ) : mNode( node ), mMatrixID( matrixID ) {
	D3DXMatrixIdentity( &mMatrix );
}
