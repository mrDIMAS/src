#pragma once

#include "Common.h"

class Skybox
{
public:
  Texture * up;
  Texture * left;
  Texture * right;
  Texture * forw;
  Texture * back;  

  IDirect3DVertexBuffer9 * vbUp;
  IDirect3DVertexBuffer9 * vbLeft;
  IDirect3DVertexBuffer9 * vbRight;
  IDirect3DVertexBuffer9 * vbForw;
  IDirect3DVertexBuffer9 * vbBack;

  IDirect3DIndexBuffer9 * ib;

  explicit Skybox( const char * path );
  virtual ~Skybox();
  void CreateIndexBuffer( size_t size, IDirect3DIndexBuffer9 ** ib );
  void CreateVertexBuffer( size_t size, IDirect3DVertexBuffer9 ** vb );
  void WriteToVertexBuffer( IDirect3DVertexBuffer9 * vb, vector< Vertex > & vertices );
  void WriteToIndexBuffer( IDirect3DIndexBuffer9 * ib, vector< unsigned short > & indices );

  void Render( const btVector3 & pos );
};
