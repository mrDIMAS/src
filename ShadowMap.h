#pragma once

#include "Common.h"
#include "Light.h"
#include "Shader.h"
#include "Mesh.h"
#include "Utility.h"

class ShadowMap {
private:
    IDirect3DTexture9 * spotShadowMap;
    IDirect3DCubeTexture9 * pointShadowMap;
    IDirect3DSurface9 * spotSurface;
    IDirect3DSurface9 * defaultDepthStencil;
    IDirect3DSurface9 * depthStencil;
    PixelShader * pixelShader;
    VertexShader * vertexShader;
public:
    ShadowMap( float size = 1024 );
    ~ShadowMap();
    void RenderSpotShadowMap( IDirect3DSurface9 * lastUsedRT, int rtIndex, Light * spotLight );
    void BindSpotShadowMap( int index );
    void UnbindSpotShadowMap( int index );
};