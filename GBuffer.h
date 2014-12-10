#pragma once

#include "Common.h"

#define USE_R32F_DEPTH

class GBuffer
{
public:
    IDirect3DTexture9 * depthMap;
    IDirect3DTexture9 * normalMap;
    IDirect3DTexture9 * diffuseMap;

    IDirect3DSurface9 * depthSurface;
    IDirect3DSurface9 * normalSurface;
    IDirect3DSurface9 * diffuseSurface;
    IDirect3DSurface9 * backSurface;

public:
    GBuffer();
    ~GBuffer();

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