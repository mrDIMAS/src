#pragma once

#include "DeferredRenderer.h"

class MultipleRTDeferredRenderer : public DeferredRenderer {
private:
	// Standard GBuffer shader
	VertexShader * mVertexShader;
	PixelShader * mPixelShader;

	// Parallax occlusion mapping shaders
	VertexShader * mVertexShaderPOM;
	PixelShader * mPixelShaderPOM;	

	VertexShader * mCurrentVertexShader;
	PixelShader * mCurrentPixelShader;

	bool mUsePOM;
public:
    explicit MultipleRTDeferredRenderer( bool usePOM );
    virtual ~MultipleRTDeferredRenderer();
	virtual void BindParallaxShaders() {
		mPixelShaderPOM->Bind();
		mVertexShaderPOM->Bind();
	}
	virtual void BindGenericShaders() {
		mPixelShader->Bind();
		mVertexShader->Bind();
	}
	void SetPOMEnabled( bool state );
    void BeginFirstPass();
    void RenderMesh( Mesh * mesh );
    void OnEnd();
};