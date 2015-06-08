#pragma once



#define USE_R32F_DEPTH

class GBuffer : public RendererComponent {
public:
    IDirect3DTexture9 * depthMap;
    IDirect3DTexture9 * normalMap;
    IDirect3DTexture9 * diffuseMap;

    IDirect3DSurface9 * depthSurface;
    IDirect3DSurface9 * normalSurface;
    IDirect3DSurface9 * diffuseSurface;
    IDirect3DSurface9 * backSurface;

	void CreateRenderTargets();
	void FreeRenderTargets();
public:
    explicit GBuffer();
    virtual ~GBuffer();

	void OnResetDevice();
	void OnLostDevice();
    void BindRenderTargets();
    void UnbindRenderTargets();
    void BindTextures();
    void BindDepthMap( int layer );

    void BindNormalMapAsRT();
    void BindDiffuseMapAsRT();
    void BindDepthMapAsRT();
    void BindBackSurfaceAsRT();
    void UnbindTextures();
};