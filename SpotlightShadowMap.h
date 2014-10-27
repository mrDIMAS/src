#pragma once

#include "Common.h"
#include "Light.h"
#include "Shader.h"
#include "Mesh.h"
#include "Utility.h"

class SpotlightShadowMap {
private:
    IDirect3DTexture9 * spotShadowMap;
    IDirect3DSurface9 * spotSurface;
    IDirect3DSurface9 * defaultDepthStencil;
    IDirect3DSurface9 * depthStencil;
    PixelShader * pixelShader;
    VertexShader * vertexShader;
public:
    explicit SpotlightShadowMap( float size = 1024 );
    virtual ~SpotlightShadowMap();
    void RenderSpotShadowMap( IDirect3DSurface9 * lastUsedRT, int rtIndex, Light * spotLight );
    void BindSpotShadowMap( int index );
    void UnbindSpotShadowMap( int index );
};