#include "Mesh.h"
#include "Octree.h"
#include "Texture.h"
#include "Vertex.h"

unordered_map< IDirect3DTexture9*, vector< Mesh*>> Mesh::meshes;

Mesh::Mesh( SceneNode * theParent )
{
  diffuseTexture = 0;
  indexBuffer = 0;
  vertexBuffer = 0;
  parent = theParent;    
  normalMapTexture = 0;
  octree = 0;

  boundingVolume.max = Vector3( FLT_MAX, FLT_MAX, FLT_MAX );
  boundingVolume.min = Vector3( FLT_MIN, FLT_MIN, FLT_MIN );
  boundingVolume.center = Vector3( 0.0f, 0.0f, 0.0f );
  boundingVolume.radius = FLT_MAX;
}

void Mesh::Register( Mesh * mesh )
{
  auto textureGroup = Mesh::meshes.find( mesh->diffuseTexture->GetInterface() );

  if( textureGroup == Mesh::meshes.end())
    Mesh::meshes[ mesh->diffuseTexture->GetInterface() ] = vector< Mesh*>();

  Mesh::meshes[ mesh->diffuseTexture->GetInterface() ].push_back( mesh );
}

Mesh::~Mesh()
{
  if( indexBuffer )
    indexBuffer->Release();

  if( vertexBuffer )
    vertexBuffer->Release();

  auto group = Mesh::meshes.find( diffuseTexture->GetInterface() );

  if( group != Mesh::meshes.end())
  {
    auto & meshes = group->second;

    for( size_t i = 0; i < meshes.size(); i++ )
    {
      if( meshes.at( i ) == this )
      {
        meshes.erase( meshes.begin() + i );
      }
    }
  }

  if( octree )
  {
    delete octree;
  }
}

void Mesh::UpdateVertexBuffer()
{
  int sizeBytes = vertices.size() * sizeof( Vertex );

  if( !vertexBuffer )
    g_device->CreateVertexBuffer( sizeBytes, D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, D3DPOOL_DEFAULT, &vertexBuffer, 0 );

  if( vertices.size() == 0 )
    return;

  void * vertexData = 0;  

  vertexBuffer->Lock( 0, 0, &vertexData, 0 );

  memcpy( vertexData, &vertices[ 0 ], sizeBytes );

  vertexBuffer->Unlock();
}

void Mesh::UpdateIndexBuffer( vector< Triangle > & idc )
{
  int sizeBytes = idc.size() * 3 * sizeof( unsigned short );

  if( !indexBuffer )
    g_device->CreateIndexBuffer( sizeBytes,D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, 0 );

  void * indexData = 0;

  indexBuffer->Lock( 0, 0, &indexData, 0 );

  memcpy( indexData, &idc[ 0 ], sizeBytes );

  indexBuffer->Unlock();
}

SceneNode * Mesh::GetParentNode()
{
  return parent;
}

void Mesh::UpdateBuffers()
{
  UpdateVertexBuffer();
  UpdateIndexBuffer( triangles );
}

Texture * Mesh::GetDiffuseTexture()
{
  return diffuseTexture;
}

Texture * Mesh::GetNormalTexture()
{
  return normalMapTexture;
}

void Mesh::BindBuffers()
{
  g_device->SetVertexDeclaration( g_meshVertexDeclaration );
  g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( Vertex ));

  if( octree )
  {
    vector< Triangle > & id = octree->GetTrianglesToRender();

    if( id.size() )
      UpdateIndexBuffer( id );
  }

  g_device->SetIndices( indexBuffer );
}

void Mesh::Render()
{
  if( octree )
  {
#ifdef _OCTREE_DEBUG
    octree->VisualizeHierarchy();
    DrawGUIText( Format( "Nodes: %d, Triangles: %d", octree->visibleNodes, octree->visibleTriangles ).c_str(), 40, 40, 100, 50, g_font, Vector3( 255, 0, 0 ), 1 );
#endif

    if( octree->visibleTris.size() )
      g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertices.size(), 0, octree->visibleTris.size() );
  }
  else
  {
    g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertices.size(), 0, triangles.size() );
  }
}