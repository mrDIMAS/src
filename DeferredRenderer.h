/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

#pragma once

#include "Shader.h"
#include "GBuffer.h"
#include "EffectsQuad.h"
#include "FXAA.h"
#include "SpotlightShadowMap.h"
#include "HDR.h"
#include "Postprocessing.h"
#include "PointLight.h"
#include "SpotLight.h"

class Mesh;

class DeferredRenderer : public RendererComponent {
protected:
    class BoundingVolumeRenderingShader : public RendererComponent {
    private:
        unique_ptr<PixelShader> mPixelShader;
        COMPtr<IDirect3DVertexDeclaration9> mVertexDeclaration;
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
        unique_ptr<PixelShader> pixelShader;
    public:
        AmbientLightShader();
        ~AmbientLightShader();
        void Bind( );
    };

    // Point Light
    class PointLightShader {
	public:
        unique_ptr<PixelShader> mPixelShader;
		unique_ptr<PixelShader> mPixelShaderTexProj;
    public:
        PointLightShader();
        ~PointLightShader();

        void SetLight( D3DXMATRIX & invViewProj, const shared_ptr<PointLight> & lit, int noiseTexSize );
    };

    // Spot Light
    class SpotLightShader {
    private:
        unique_ptr<PixelShader> mPixelShader;
		unique_ptr<PixelShader> mPixelShaderShadows;		
    public:
        SpotLightShader( );
        ~SpotLightShader();
        void SetLight( D3DXMATRIX & invViewProj, const shared_ptr<SpotLight> & lit, int noiseTexSize );
    };

	class SkyboxShader {
	private:
		unique_ptr<VertexShader> mVertexShader;
		unique_ptr<PixelShader> mPixelShader;
		
	public:
		SkyboxShader();
		~SkyboxShader();
		void Bind( const ruVector3 & position );
	};

	unique_ptr<BoundingVolumeRenderingShader> bvRenderer;
	unique_ptr<EffectsQuad> mFullscreenQuad;
    unique_ptr<GBuffer> mGBuffer;
    unique_ptr<AmbientLightShader> mAmbientLightShader;
    unique_ptr<PointLightShader> mPointLightShader;
    unique_ptr<SpotLightShader> mSpotLightShader;
	unique_ptr<SkyboxShader> mSkyboxShader;
    unique_ptr<FXAA> mFXAA;
    unique_ptr<SpotlightShadowMap> mSpotLightShadowMap;
    unique_ptr<HDRShader> mHDRShader;
	unique_ptr<Postprocessing> mPostprocessing;

    void CreateBoundingVolumes();

    void RenderSphere( const shared_ptr<PointLight> & pLight, float scale = 1.0f );
	void RenderStar( shared_ptr<PointLight> pLight, float scale = 1.0f );
    void RenderCone( shared_ptr<SpotLight> lit );
    void RenderMeshShadow( Mesh * mesh );

	// !!! do not change order of these !!!
	COMPtr<IDirect3DTexture9> mHDRFrame;
	COMPtr<IDirect3DSurface9> mHDRFrameSurface;

	COMPtr<IDirect3DTexture9> mFrame[2];
	COMPtr<IDirect3DSurface9> mFrameSurface[2];

	COMPtr<IDirect3DSurface9> mBackBufferSurface;

	COMPtr<ID3DXMesh> mBoundingSphere;
	COMPtr<ID3DXMesh> mBoundingStar;
	COMPtr<ID3DXMesh> mBoundingCone;

	int mNoiseTextureSize;
	COMPtr<IDirect3DTexture9> mNoiseTexture;
public:
    explicit DeferredRenderer();
    virtual ~DeferredRenderer();

	virtual void BeginFirstPass() = 0;
    virtual void RenderMesh( shared_ptr<Mesh> mesh ) = 0;
    virtual void OnEnd() = 0;
	virtual void BindParallaxShaders() = 0;
	virtual void BindGenericShaders() = 0;
	virtual void BindGenericSkinShaders() = 0;
    void SetSpotLightShadowMapSize( int size );
    void DoLightingAndPostProcessing();

	void OnResetDevice();
	void OnLostDevice();
};
