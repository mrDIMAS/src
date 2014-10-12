#pragma once

#include "Common.h"

class Texture
{
private:
  explicit Texture( );
  IDirect3DTexture9 * texture;
public:
  
  string name;

  static map< string, Texture* > all;
  
  virtual ~Texture( );

  void Bind( int level );
  IDirect3DTexture9 * GetInterface();
  static Texture * Require( string file );
  static void DeleteAll();
};