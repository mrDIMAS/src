#pragma once

#include "Shader.h"
#include "EffectsQuad.h"

class FXAA
{
public:
  PixelShader * pixelShader;
  EffectsQuad * effectsQuad;
  IDirect3DSurface9 * renderTarget;
  IDirect3DSurface9 * backBufferRT;
  IDirect3DTexture9 * texture;
  D3DXHANDLE screenWidth;
  D3DXHANDLE screenHeight;

  FXAA();
  ~FXAA()
  {
    delete pixelShader;
    delete effectsQuad;
    renderTarget->Release();
    texture->Release();
  }

  void BeginDrawIntoTexture( );

  void DoAntialiasing( IDirect3DTexture9 * outTexture );
};