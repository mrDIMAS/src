#pragma once

#include "Common.h"
#include "Shader.h"
#include "SceneNode.h"
#include "Mesh.h"

class LightmapRenderer
{
private:
  PixelShader * pixelShader;
  VertexShader * vertexShader;

  D3DXHANDLE vWVP;
  D3DXHANDLE vView;
  D3DXHANDLE vProj;

public:
  LightmapRenderer();

  void Begin();

  void RenderMesh( Mesh * mesh );

  void End();
};