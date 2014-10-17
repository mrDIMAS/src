#pragma once

#include "DeferredRenderer.h"

class MultipleRTDeferredRenderer : public DeferredRenderer {
public:
    VertexShader * vertexShaderPassOne;
    PixelShader * pixelShaderPassOne;

    D3DXHANDLE v1World;
    D3DXHANDLE v1WVP;

    D3DXHANDLE pAlbedo;

    void InitPassOneShaders();

    MultipleRTDeferredRenderer();
    ~MultipleRTDeferredRenderer();

    void BeginFirstPass();

    void RenderMesh( Mesh * mesh );

    void OnEnd();
};