#include "Skybox.h"
#include "Texture.h"
#include "Vertex.h"

void Skybox::WriteToIndexBuffer( IDirect3DIndexBuffer9 * ib, vector< unsigned short > & indices )
{
  void * indexData = 0;

  ib->Lock( 0, 0, &indexData, 0 );

  memcpy( indexData, &indices[ 0 ], indices.size() * sizeof( unsigned short ) );

  ib->Unlock();
}

void Skybox::WriteToVertexBuffer( IDirect3DVertexBuffer9 * vb, vector< Vertex > & vertices )
{
  void * vertexData = 0;  

  vb->Lock( 0, 0, &vertexData, 0 );

  memcpy( vertexData, &vertices[ 0 ], vertices.size() * sizeof( Vertex ) );

  vb->Unlock();
}

void Skybox::CreateVertexBuffer( size_t size, IDirect3DVertexBuffer9 ** vb )
{
  g_device->CreateVertexBuffer( size,D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_DEFAULT, vb, 0 );
}

void Skybox::CreateIndexBuffer( size_t size, IDirect3DIndexBuffer9 ** ib )
{
  g_device->CreateIndexBuffer( size,D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, ib, 0 );
}

Skybox::~Skybox()
{
  vbBack->Release();
  vbForw->Release();
  vbRight->Release();
  vbLeft->Release();
  vbUp->Release();

  ib->Release();
}

Skybox::Skybox( const char * path )
{
  string file = path;

  string leftPath = file;
  leftPath += "_l.jpg";
  left = Texture::Require( leftPath.c_str() );

  string rightPath = file;
  rightPath += "_r.jpg";
  right = Texture::Require( rightPath.c_str() );

  string frontPath = file;
  frontPath += "_f.jpg";
  forw = Texture::Require( frontPath.c_str() );

  string backPath = file;
  backPath += "_b.jpg";
  back = Texture::Require( backPath.c_str() );

  string upPath = file;
  upPath += "_u.jpg";
  up = Texture::Require( upPath.c_str() );

  float size = 2048.0f;

  vector< Vertex > fV;
  vector< Vertex > bV;
  vector< Vertex > uV;
  vector< Vertex > rV;
  vector< Vertex > lV;

  fV.push_back ( Vertex ( -size,  size, -size, 0.0f, 0.0f ) );
  fV.push_back ( Vertex (  size,  size, -size, 1.0f, 0.0f ) );
  fV.push_back ( Vertex (  size, -size, -size, 1.0f, 1.0f ) );
  fV.push_back ( Vertex ( -size, -size, -size, 0.0f, 1.0f ) );

  bV.push_back ( Vertex (  size,  size,  size, 0.0f, 0.0f ) );
  bV.push_back ( Vertex ( -size,  size,  size, 1.0f, 0.0f ) );
  bV.push_back ( Vertex ( -size, -size,  size, 1.0f, 1.0f ) );
  bV.push_back ( Vertex (  size, -size,  size, 0.0f, 1.0f ) );

  rV.push_back ( Vertex (  size,  size, -size, 0.0f, 0.0f ) );
  rV.push_back ( Vertex (  size,  size,  size, 1.0f, 0.0f ) );
  rV.push_back ( Vertex (  size, -size,  size, 1.0f, 1.0f ) );
  rV.push_back ( Vertex (  size, -size, -size, 0.0f, 1.0f ) );

  lV.push_back ( Vertex ( -size,  size,  size, 0.0f, 0.0f ) );
  lV.push_back ( Vertex ( -size,  size, -size, 1.0f, 0.0f ) );
  lV.push_back ( Vertex ( -size, -size, -size, 1.0f, 1.0f ) );
  lV.push_back ( Vertex ( -size, -size,  size, 0.0f, 1.0f ) );

  uV.push_back ( Vertex ( -size,  size,  size, 0.0f, 0.0f ) );
  uV.push_back ( Vertex (  size,  size,  size, 1.0f, 0.0f ) );
  uV.push_back ( Vertex (  size,  size, -size, 1.0f, 1.0f ) );
  uV.push_back ( Vertex ( -size,  size, -size, 0.0f, 1.0f ) );

  CreateVertexBuffer( fV.size() * sizeof( Vertex ), &vbForw );
  WriteToVertexBuffer( vbForw, fV );

  CreateVertexBuffer( bV.size() * sizeof( Vertex ), &vbBack );
  WriteToVertexBuffer( vbBack, bV );

  CreateVertexBuffer( rV.size() * sizeof( Vertex ), &vbRight );
  WriteToVertexBuffer( vbRight, rV );

  CreateVertexBuffer( lV.size() * sizeof( Vertex ), &vbLeft );
  WriteToVertexBuffer( vbLeft, lV );

  CreateVertexBuffer( uV.size() * sizeof( Vertex ), &vbUp );
  WriteToVertexBuffer( vbUp, uV );

  vector< unsigned short > indices;
  indices.push_back ( 0 );
  indices.push_back ( 1 );
  indices.push_back ( 2 );
  indices.push_back ( 0 );
  indices.push_back ( 2 );
  indices.push_back ( 3 );

  CreateIndexBuffer( sizeof( Vertex ) * indices.size(), &ib );
  WriteToIndexBuffer( ib, indices );
}

void Skybox::Render( const btVector3 & pos )
{
  IDirect3DStateBlock9 * state;
  g_device->CreateStateBlock( D3DSBT_ALL, &state );

  g_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
  g_device->SetRenderState ( D3DRS_ZENABLE, TRUE );
  g_device->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );

  g_device->SetVertexShader ( 0 );
  g_device->SetPixelShader ( 0 );
  g_device->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );

  g_device->SetSamplerState ( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
  g_device->SetSamplerState ( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

  D3DXMATRIX view;
  D3DXMatrixTranslation ( &view, pos.x(), pos.y(), pos.z() );
  g_device->SetTransform ( D3DTS_WORLD, &view );

  g_device->SetIndices( ib );
  forw->Bind( 0 );
  g_device->SetStreamSource( 0, vbForw, 0, sizeof( Vertex ));
  g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );

  back->Bind( 0 );
  g_device->SetStreamSource( 0, vbBack, 0, sizeof( Vertex ));
  g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );

  right->Bind( 0 );
  g_device->SetStreamSource( 0, vbRight, 0, sizeof( Vertex ));
  g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );

  left->Bind( 0 );
  g_device->SetStreamSource( 0, vbLeft, 0, sizeof( Vertex ));
  g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );

  up->Bind( 0 );
  g_device->SetStreamSource( 0, vbUp, 0, sizeof( Vertex ));
  g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );

  state->Apply();
  state->Release();
}
