#include "Mesh.h"
#include "Octree.h"
#include "Texture.h"
#include "Vertex.h"
#include "ForwardRenderer.h"

unordered_map< IDirect3DTexture9*, vector< Mesh*>> Mesh::meshes;

Mesh::Mesh( SceneNode * theParent ) {
    diffuseTexture = 0;
    indexBuffer = 0;
    vertexBuffer = 0;
    ownerNode = theParent;
    normalMapTexture = 0;
    octree = 0;
    opacity = 1.0f;
}

void Mesh::Register( Mesh * mesh ) {
    if( mesh->opacity > 0.99f ) { // pass it to deferred renderer
        auto textureGroup = Mesh::meshes.find( mesh->diffuseTexture->GetInterface() );

        if( textureGroup == Mesh::meshes.end()) {
            Mesh::meshes[ mesh->diffuseTexture->GetInterface() ] = vector< Mesh*>();
        }

        Mesh::meshes[ mesh->diffuseTexture->GetInterface() ].push_back( mesh );
    } else { // pass it to forward renderer
        g_forwardRenderer->AddMesh( mesh );
    }
}

Mesh::~Mesh() {
    if( indexBuffer ) {
        indexBuffer->Release();
    }

    if( vertexBuffer ) {
        vertexBuffer->Release();
    }
    bool removed = false;
    auto group = Mesh::meshes.find( diffuseTexture->GetInterface() );
    if( group != Mesh::meshes.end()) {
        auto & meshes = group->second;
        for( size_t i = 0; i < meshes.size(); i++ ) {
            if( meshes[i] == this ) {
                meshes.erase( meshes.begin() + i );
                removed = true;
            }
        }
    }
    if( !removed ) {
        g_forwardRenderer->RemoveMesh( this );
    }
    if( octree ) {
        delete octree;
    }
}

void Mesh::UpdateVertexBuffer() {
    int sizeBytes = vertices.size() * sizeof( Vertex );
    if( !vertexBuffer ) {
        CheckDXErrorFatal( g_device->CreateVertexBuffer( sizeBytes, D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, D3DPOOL_DEFAULT, &vertexBuffer, 0 ));
    }
    if( vertices.size() == 0 ) {
        return;
    }
    void * vertexData = 0;
    CheckDXErrorFatal( vertexBuffer->Lock( 0, 0, &vertexData, 0 ));
    memcpy( vertexData, &vertices[ 0 ], sizeBytes );
    CheckDXErrorFatal( vertexBuffer->Unlock());
}

void Mesh::UpdateIndexBuffer( vector< Triangle > & triangles ) {
    int sizeBytes = triangles.size() * 3 * sizeof( unsigned short );
    if( !indexBuffer ) {
        CheckDXErrorFatal( g_device->CreateIndexBuffer( sizeBytes,D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, 0 ));
    }
    void * indexData = 0;
    CheckDXErrorFatal( indexBuffer->Lock( 0, 0, &indexData, 0 ));
    memcpy( indexData, &triangles[ 0 ], sizeBytes );
    CheckDXErrorFatal( indexBuffer->Unlock());
}

SceneNode * Mesh::GetParentNode() {
    return ownerNode;
}

void Mesh::UpdateBuffers() {
    UpdateVertexBuffer();
    UpdateIndexBuffer( triangles );
}

Texture * Mesh::GetDiffuseTexture() {
    return diffuseTexture;
}

Texture * Mesh::GetNormalTexture() {
    return normalMapTexture;
}

void Mesh::BindBuffers() {
    CheckDXErrorFatal( g_device->SetVertexDeclaration( g_meshVertexDeclaration ));
    CheckDXErrorFatal( g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex )));
    if( octree ) {
        vector< Triangle > & id = octree->GetTrianglesToRender();
        if( id.size() ) {
            UpdateIndexBuffer( id );
        }
    }
    CheckDXErrorFatal( g_device->SetIndices( indexBuffer ));
}

void Mesh::Render() {
    if( octree ) {
#ifdef _OCTREE_DEBUG
        octree->VisualizeHierarchy();
        DrawGUIText( Format( "Nodes: %d, Triangles: %d", octree->visibleNodes, octree->visibleTriangles ).c_str(), 40, 40, 100, 50, g_font, Vector3( 255, 0, 0 ), 1 );
#endif
        if( octree->visibleTris.size() ) {
            CheckDXErrorFatal( g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertices.size(), 0, octree->visibleTris.size() ));
        }
    } else {
        CheckDXErrorFatal( g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertices.size(), 0, triangles.size() ));
    }

    // each mesh renders in one DIP
    g_dips++;
}