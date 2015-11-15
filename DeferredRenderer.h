#pragma once

#include "Shader.h"
#include "GBuffer.h"
#include "EffectsQuad.h"
#include "FXAA.h"
#include "SpotlightShadowMap.h"
#include "HDR.h"

class Mesh;

class DeferredRenderer : public RendererComponent {
public:
    class BoundingVolumeRenderingShader : public RendererComponent {
    private:
        //VertexShader * vs;
        PixelShader * ps;

        IDirect3DVertexDeclaration9 * vertexDeclaration;

        //D3DXHANDLE vWVP;
    public:
        BoundingVolumeRenderingShader();
        ~BoundingVolumeRenderingShader();

        void Bind();
        void SetTransform( D3DXMATRIX & wvp );

		void OnResetDevice();
		void OnLostDevice();
    };



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
	public:
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

	class SkyboxShader {
	private:
		VertexShader * mVertexShader;
		PixelShader * mPixelShader;
		
	public:
		SkyboxShader();
		~SkyboxShader();
		void Bind( const btVector3 & position );
	};

	ID3DXMesh * mBoundingSphere;
	ID3DXMesh * mBoundingStar;
	ID3DXMesh * mBoundingCone;

	shared_ptr<BoundingVolumeRenderingShader> bvRenderer;
	shared_ptr<EffectsQuad> mFullscreenQuad;
    shared_ptr<GBuffer> mGBuffer;
    shared_ptr<AmbientLightShader> mAmbientLightShader;
    shared_ptr<PointLightShader> mPointLightShader;
    shared_ptr<SpotLightShader> mSpotLightShader;
	shared_ptr<SkyboxShader> mSkyboxShader;
    shared_ptr<FXAA> mFXAA;
    shared_ptr<SpotlightShadowMap> mSpotLightShadowMap;
    shared_ptr<HDRShader> mHDRShader;

    void CreateBoundingVolumes();

    void RenderSphere( Light * pLight, float scale = 1.0f );
	void RenderStar( Light * pLight, float scale = 1.0f );
    void RenderConeIntoStencilBuffer( Light * lit );
    void RenderMeshShadow( Mesh * mesh );
public:
    explicit DeferredRenderer();
    virtual ~DeferredRenderer();

    GBuffer * GetGBuffer();

    virtual void BeginFirstPass() = 0;
    virtual void RenderMesh( Mesh * mesh ) = 0;
    virtual void OnEnd() = 0;
	virtual void BindParallaxShaders() = 0;
	virtual void BindGenericShaders() = 0;
    void SetSpotLightShadowMapSize( int size );
    void EndFirstPassAndDoSecondPass();

	void OnResetDevice();
	void OnLostDevice();
};
