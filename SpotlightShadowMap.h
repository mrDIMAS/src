#pragma once


#include "Light.h"
#include "Shader.h"
#include "Mesh.h"
#include "Utility.h"

class SpotlightShadowMap : public RendererComponent {
private:
    IDirect3DTexture9 * spotShadowMap;
    IDirect3DSurface9 * spotSurface;
    IDirect3DSurface9 * defaultDepthStencil;
    IDirect3DSurface9 * depthStencil;
    PixelShader * pixelShader;
    VertexShader * vertexShader;
	void Initialize();
public:
	void OnResetDevice();
	void OnLostDevice();
    int iSize;
    explicit SpotlightShadowMap( float size = 1024 );
    virtual ~SpotlightShadowMap();
    void RenderSpotShadowMap( IDirect3DSurface9 * lastUsedRT, int rtIndex, Light * spotLight );
    void BindSpotShadowMap( int index );
    void UnbindSpotShadowMap( int index );
};