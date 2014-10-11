#pragma once

#include "Common.h"

class VertexShader
{
private:
  IDirect3DVertexShader9 * shader;
  ID3DXConstantTable * constants;
public:
  VertexShader( string source );
  ~VertexShader();
  ID3DXConstantTable * GetConstantTable();
  void Bind();
};

class PixelShader
{
private:
  IDirect3DPixelShader9 * shader;
  ID3DXConstantTable * constants;
public:
  PixelShader( string source );
  ~PixelShader();
  ID3DXConstantTable * GetConstantTable();
  void Bind();
};