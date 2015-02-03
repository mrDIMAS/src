#pragma once

#include "DeferredRenderer.h"

class MultipleRTDeferredRenderer : public DeferredRenderer {
public:
    VertexShader * mGBufferVertexShader;
    PixelShader * mGBufferPixelShader;
	
    explicit MultipleRTDeferredRenderer();
    virtual ~MultipleRTDeferredRenderer();

    void BeginFirstPass();
    void RenderMesh( Mesh * mesh );
    void OnEnd();
};