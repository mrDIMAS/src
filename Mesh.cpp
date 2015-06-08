#include "Precompiled.h"
#include "Engine.h"
#include "Mesh.h"
#include "Octree.h"
#include "Texture.h"
#include "Vertex.h"
#include "ForwardRenderer.h"

IDirect3DVertexDeclaration9 * Mesh::msVertexDeclaration = 0;
unordered_map< IDirect3DTexture9*, vector< Mesh*>> Mesh::msMeshList;

Mesh::Mesh() {
	if( !msVertexDeclaration ) {
		D3DVERTEXELEMENT9 vd[ ] = {
			{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
			D3DDECL_END()
		};

		CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateVertexDeclaration( vd, &msVertexDeclaration ));
	}
    mDiffuseTexture = nullptr;
    mIndexBuffer = nullptr;
    mVertexBuffer = nullptr;
    mNormalTexture = nullptr;
    mOctree = nullptr;
    mOpacity = 1.0f;
}

void Mesh::LinkTo( SceneNode * owner ) {
	mOwnerList.push_back( owner );
}

void Mesh::Unlink( SceneNode * owner ) {
	auto iter = find( mOwnerList.begin(), mOwnerList.end(), owner ); 
	if( iter != mOwnerList.end() ) {
		mOwnerList.erase( iter );
	}
}

void Mesh::Register( Mesh * mesh ) {
    if( mesh->mOpacity > 0.99f ) { // pass it to deferred renderer
        auto textureGroup = Mesh::msMeshList.find( mesh->mDiffuseTexture->GetInterface() );

        if( textureGroup == Mesh::msMeshList.end()) {
            Mesh::msMeshList[ mesh->mDiffuseTexture->GetInterface() ] = vector< Mesh*>();
        }

        Mesh::msMeshList[ mesh->mDiffuseTexture->GetInterface() ].push_back( mesh );
    } else { // pass it to forward renderer
        Engine::Instance().GetForwardRenderer()->AddMesh( mesh );
    }
}

Mesh::~Mesh() {
    OnLostDevice();
    
    if( mOctree ) {
        delete mOctree;
    }
}

void Mesh::UpdateVertexBuffer() {
    int sizeBytes = mVertices.size() * sizeof( Vertex );
    if( !mVertexBuffer ) {
        CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateVertexBuffer( sizeBytes, D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, D3DPOOL_DEFAULT, &mVertexBuffer, 0 ));
    }
    if( mVertices.size() == 0 ) {
        return;
    }
    void * vertexData = 0;
    CheckDXErrorFatal( mVertexBuffer->Lock( 0, 0, &vertexData, 0 ));
    memcpy( vertexData, &mVertices[ 0 ], sizeBytes );
    CheckDXErrorFatal( mVertexBuffer->Unlock());
}

void Mesh::UpdateIndexBuffer( vector< Triangle > & triangles ) {
    int sizeBytes = triangles.size() * 3 * sizeof( unsigned short );
    if( !mIndexBuffer ) {
        CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateIndexBuffer( sizeBytes,D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mIndexBuffer, 0 ));
    }
    void * indexData = 0;
    CheckDXErrorFatal( mIndexBuffer->Lock( 0, 0, &indexData, 0 ));
    memcpy( indexData, &triangles[ 0 ], sizeBytes );
    CheckDXErrorFatal( mIndexBuffer->Unlock());
}

vector<SceneNode*> & Mesh::GetOwners() {
    return mOwnerList;
}

void Mesh::UpdateBuffers() {
    UpdateVertexBuffer();
    UpdateIndexBuffer( mTriangles );
}

void Mesh::EraseOrphanMeshes() {
	for( auto iMeshGroup : msMeshList ) {
		for( auto iMesh = iMeshGroup.second.begin(); iMesh != iMeshGroup.second.end(); ) {
			Mesh * pMesh = *iMesh;
			if( pMesh->GetOwners().size() == 0 ) {
				delete pMesh;
				iMesh = iMeshGroup.second.erase( iMesh );
			} else {
				iMesh++;
			}
		}
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
}

Texture * Mesh::GetDiffuseTexture() {
    return mDiffuseTexture;
}

Texture * Mesh::GetNormalTexture() {
    return mNormalTexture;
}

void Mesh::BindBuffers() {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetVertexDeclaration( msVertexDeclaration ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetStreamSource( 0, mVertexBuffer, 0, sizeof( Vertex )));
    if( mOctree ) {
        vector< Triangle > & id = mOctree->GetTrianglesToRender();
        if( id.size() ) {
            UpdateIndexBuffer( id );
        }
    }
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetIndices( mIndexBuffer ));
}

void Mesh::Render() {
    if( mOctree ) {
#ifdef _OCTREE_DEBUG
        mOctree->VisualizeHierarchy();
        ruDrawGUIText( Format( "Nodes: %d, Triangles: %d", mOctree->mVisibleNodeCount, mOctree->mVisibleTriangleCount ).c_str(), 40, 40, 100, 50, g_font, ruVector3( 255, 0, 0 ), 1 );
#endif
        if( mOctree->mVisibleTriangleList.size() ) {
            CheckDXErrorFatal( Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, mVertices.size(), 0, mOctree->mVisibleTriangleList.size() ));
        }
    } else {
        CheckDXErrorFatal( Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, mVertices.size(), 0, mTriangles.size() ));
    }

    // each mesh renders in one DIP
    Engine::Instance().RegisterDIP();
}

void Mesh::OnLostDevice()
{
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
		Engine::Instance().GetForwardRenderer()->RemoveMesh( this );
	}
	mVertexBuffer->Release();
	mIndexBuffer->Release();
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;	

}

void Mesh::OnResetDevice()
{
	UpdateBuffers();
	Mesh::Register( this );
}

Mesh::Triangle::Triangle( unsigned short vA, unsigned short vB, unsigned short vC )
{
	mA = vA;
	mB = vB;
	mC = vC;
}
