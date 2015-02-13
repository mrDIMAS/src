#pragma once

#include "Shader.h"
#include "GBuffer.h"
#include "EffectsQuad.h"
#include "FXAA.h"
#include "SpotlightShadowMap.h"
#include "HDR.h"

class Mesh;

class DeferredRenderer {
public:
    class BoundingVolumeRenderingShader {
    private:
        VertexShader * vs;
        PixelShader * ps;

        IDirect3DVertexDeclaration9 * vertexDeclaration;

        D3DXHANDLE vWVP;
    public:
        BoundingVolumeRenderingShader();
        ~BoundingVolumeRenderingShader();

        void Bind();
        void SetTransform( D3DXMATRIX & wvp );
    };

    BoundingVolumeRenderingShader * bvRenderer;

    ID3DXMesh * icosphere;
    ID3DXMesh * cone;


    // Ambient Light
    class AmbientLightShader {
    private:
        PixelShader * pixelShader;
    public:
        AmbientLightShader();
        ~AmbientLightShader();
        void Bind( );
    };

    // Point Light
    class PointLightShader {
    private:
        PixelShader * pixelShader;
		PixelShader * pixelShaderTexProj;
    public:
        PointLightShader();
        ~PointLightShader();

        void SetLight( D3DXMATRIX & invViewProj, Light * lit );
    };

    // Spot Light
    class SpotLightShader {
    private:
        PixelShader * pixelShader;
		PixelShader * pixelShaderShadows;		
    public:
        SpotLightShader( );
        ~SpotLightShader();
        void SetLight( D3DXMATRIX & invViewProj, Light * lit );
    };

    EffectsQuad * mFullscreenQuad;
    EffectsQuad * mDebugQuad;

    GBuffer * mGBuffer;

    AmbientLightShader * mAmbientLightShader;
    PointLightShader * mPointLightShader;
    SpotLightShader * mSpotLightShader;

    FXAA * mFXAA;
    SpotlightShadowMap * mSpotLightShadowMap;
    HDRShader * mHDRShader;

    void CreateBoundingVolumes();

    void RenderIcosphereIntoStencilBuffer( Light * pLight );
    void RenderConeIntoStencilBuffer( Light * lit );
    void RenderMeshShadow( Mesh * mesh );
public:
    explicit DeferredRenderer();
    virtual ~DeferredRenderer();

    GBuffer * GetGBuffer();

    virtual void BeginFirstPass() = 0;
    virtual void RenderMesh( Mesh * mesh ) = 0;
    virtual void OnEnd() = 0;

    void SetSpotLightShadowMapSize( int size );
    void EndFirstPassAndDoSecondPass();

	void OnDeviceLost() {

		mGBuffer->OnDeviceLost();
	}
};
