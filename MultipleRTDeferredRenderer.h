#pragma once

#include "DeferredRenderer.h"

class MultipleRTDeferredRenderer : public DeferredRenderer
{
public:
    VertexShader * vsGBufferHighQuality;
    PixelShader * psGBufferHighQuality;
    VertexShader * vsGBufferLowQuality;
    PixelShader * psGBufferLowQuality;
    void CreateHighQualityShaders();
    void CreateLowQualityShaders();

    explicit MultipleRTDeferredRenderer();
    virtual ~MultipleRTDeferredRenderer();

    void BeginFirstPass();
    void RenderMesh( Mesh * mesh );
    void OnEnd();
};