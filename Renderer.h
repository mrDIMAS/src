/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
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


#include "Timer.h"
#include "BitmapFont.h"
#include "Mesh.h"
#include "GUIRect.h"
#include "Camera.h"

class Cursor;

class Videomode {
public:
	int mWidth;
	int mHeight;
	int mRefreshRate;

	Videomode(int width, int height, int refreshRate) {
		mWidth = width;
		mHeight = height;
		mRefreshRate = refreshRate;
	}
};

typedef unordered_map<IDirect3DTexture9*, vector<weak_ptr<Mesh>>> MeshMap;

class Renderer {
private:
	HWND mWindowHandle;

	bool mUsePointLightShadows;
	bool mUseSpotLightShadows;
	bool mRunning;
	bool mAnisotropicFiltering;
	float mResWidth;
	float mResHeight;

	D3DPRESENT_PARAMETERS mPresentParameters;

	string mTextureStoragePath;
	ruVector3 mAmbientColor;

	bool mPaused;
	bool mChangeVideomode;
	int mNativeResolutionWidth;
	int mNativeResolutionHeight;
	void SetDefaults();
	vector<Videomode> mVideomodeList;

	shared_ptr<Cursor> mCursor;

	MeshMap mDeferredMeshMap;

	void RenderMesh(const shared_ptr<Mesh> & mesh);

	// Shader Stuff
	DWORD * ReadEntireFile(const char * fileName);
	void LoadPixelShader(COMPtr<IDirect3DPixelShader9> & pixelShader, const char * fileName);
	void LoadVertexShader(COMPtr<IDirect3DVertexShader9> & vertexShader, const char * fileName);

	//**************************************
	// Forward Rendering Stuff
	COMPtr<IDirect3DPixelShader9> mDeferredBlending;

	//**************************************
	// Deferred Rendering Stuff

	COMPtr<IDirect3DPixelShader9> mAmbientPixelShader;
	COMPtr<IDirect3DPixelShader9> mFXAAPixelShader;

	COMPtr<IDirect3DPixelShader9> mDeferredLightShader;

	// G-Buffer Stuff
	COMPtr<IDirect3DTexture9> mDepthMap;
	COMPtr<IDirect3DTexture9> mNormalMap;
	COMPtr<IDirect3DTexture9> mDiffuseMap;

	COMPtr<IDirect3DSurface9> mDepthSurface;
	COMPtr<IDirect3DSurface9> mNormalSurface;
	COMPtr<IDirect3DSurface9> mDiffuseSurface;

	// HDR Frame Buffer
	COMPtr<IDirect3DTexture9> mHDRFrame;
	COMPtr<IDirect3DSurface9> mHDRFrameSurface;

	// Two Frame Buffers for Post-effects
	COMPtr<IDirect3DTexture9> mFrame[2];
	COMPtr<IDirect3DSurface9> mFrameSurface[2];

	COMPtr<ID3DXMesh> mBoundingSphere;
	COMPtr<ID3DXMesh> mBoundingStar;
	COMPtr<ID3DXMesh> mBoundingCone;

	// GBuffer shaders
	COMPtr<IDirect3DVertexShader9> mGBufferVertexShader;
	COMPtr<IDirect3DPixelShader9> mGBufferPixelShader;

	// Particle shaders
	COMPtr<IDirect3DPixelShader9> mParticleSystemPixelShader;
	COMPtr<IDirect3DVertexShader9> mParticleSystemVertexShader;

	// HDR Stuff
	static const int mHDRDownSampleCount = 6;

	COMPtr<IDirect3DPixelShader9> mToneMapShader;
	COMPtr<IDirect3DPixelShader9> mDownScalePixelShader;
	COMPtr<IDirect3DPixelShader9> mAdaptationPixelShader;
	COMPtr<IDirect3DPixelShader9> mScaleScenePixelShader;
	COMPtr<IDirect3DTexture9> mScaledScene;
	COMPtr<IDirect3DSurface9> mScaledSceneSurf;
	COMPtr<IDirect3DTexture9> mDownSampTex[mHDRDownSampleCount];
	COMPtr<IDirect3DSurface9> mDownSampSurf[mHDRDownSampleCount];
	COMPtr<IDirect3DTexture9> mAdaptedLuminanceLast;
	COMPtr<IDirect3DTexture9> mAdaptedLuminanceCurrent;

	// Bloom stuff
	COMPtr<IDirect3DTexture9> mBloomTexture;
	COMPtr<IDirect3DSurface9> mBloomTextureSurface;
	COMPtr<IDirect3DTexture9> mBloomBlurredTexture;
	COMPtr<IDirect3DSurface9> mBloomBlurredSurface;

	COMPtr<IDirect3DPixelShader9> mBloomPixelShader;
	COMPtr<IDirect3DPixelShader9> mGaussianBlurShader;
	ruVector2 mBloomDX;
	ruVector2 mBloomDY;

	COMPtr<IDirect3DVolumeTexture9> mColorMap;

	// Shadow cubemap
	//COMPtr<IDirect3DCubeTexture9> mCubeShadowMap;
	COMPtr<IDirect3DSurface9> mCubeDepthStencilSurface;

	// Shadowmap cache: 
	//	- 1024: [0;4]   (4 total) - R16F - 12     Mbytes total
	//  - 512:  [4;8]   (4 total) - R16F - 3      Mbytes total
	//  - 256:  [8;12]  (4 total) - R16F - 0.75   Mbytes total
	//  - 128:  [12;16] (4 total) - R16F - 0.1875 Mbytes total
	//  - 64:   [16;24] (8 total) - R16F - 0.0468 Mbytes total
	//  - 32:   [24;32] (8 total) - R16F - 0.0234 Mbytes total
	//--------------------------------------------------------
	// Total memory consumption: ~16 Mbytes
	constexpr static int mShadowMapCacheSize = 32;
	COMPtr<IDirect3DCubeTexture9> mCubeShadowMapCache[32];

	// Spot light shadow map
	COMPtr<IDirect3DTexture9> mShadowMap;
	COMPtr<IDirect3DSurface9> mShadowMapSurface;
	COMPtr<IDirect3DSurface9> mDefaultDepthStencil;
	COMPtr<IDirect3DSurface9> mDepthStencilSurface;

	// Shadow map shaders
	COMPtr<IDirect3DPixelShader9> mShadowMapPixelShader;
	COMPtr<IDirect3DVertexShader9> mShadowMapVertexShader;
	
	// Quad
	D3DXMATRIX mOrthoProjectionMatrix;
	COMPtr<IDirect3DVertexBuffer9> mQuadVertexBuffer;
	COMPtr<IDirect3DVertexShader9> mQuadVertexShader;

	// SSAO Stuff
	COMPtr<IDirect3DPixelShader9> mSSAOPixelShader;

	void RenderFullscreenQuad();

	// Skybox 
	COMPtr<IDirect3DVertexBuffer9> mSkyboxVertexBuffer;
	COMPtr<IDirect3DIndexBuffer9> mSkyboxIndexBuffer;
	COMPtr<IDirect3DVertexShader9> mSkyboxVertexShader;
	COMPtr<IDirect3DPixelShader9> mSkyboxPixelShader;

	// Light flare stuff
	COMPtr<IDirect3DVertexShader9> mFlareVertexShader;
	COMPtr<IDirect3DPixelShader9> mFlarePixelShader;
	COMPtr<IDirect3DVertexBuffer9> mFlareVertexBuffer;
	COMPtr<IDirect3DIndexBuffer9> mFlareIndexBuffer;
	COMPtr<IDirect3DTexture9> mFlareTexture;

	// Common vertex declaration
	COMPtr<IDirect3DVertexDeclaration9> msVertexDeclaration;

	// GUI Stuff
	COMPtr<IDirect3DVertexBuffer9> mRectVertexBuffer;
	COMPtr<IDirect3DVertexShader9> mGUIVertexShader;
	COMPtr<IDirect3DPixelShader9> mGUIPixelShader;
	COMPtr<IDirect3DVertexBuffer9> mTextVertexBuffer;
	COMPtr<IDirect3DIndexBuffer9> mTextIndexBuffer;
	int mTextMaxChars;

	void RenderRect(const shared_ptr<GUIRect> &);

	// Default textures
	COMPtr<IDirect3DCubeTexture9> mWhiteCubeMap;
	COMPtr<IDirect3DTexture9> mDefaultNormalMap;
	COMPtr<IDirect3DTexture9> mWhiteMap;

	// D3D9
	COMPtr<IDirect3D9Ex> mpDirect3D;
	COMPtr<IDirect3DDevice9Ex> mpDevice;
	COMPtr<IDirect3DSurface9> mpBackBuffer;

	// Rendering statistics
	int mDIPCount;
	int mTextureChangeCount;
	int mShadersChangeCount;
	int mRenderedTriangleCount;

	// Effects control
	bool mHDREnabled;
	bool mParallaxEnabled;
	bool mFXAAEnabled;
	bool mSSAOEnabled;
public:
	explicit Renderer(int width, int height, int fullscreen, char vSync);
	virtual ~Renderer();
	void RenderWorld();
	void UpdateWorld();
	float GetResolutionWidth();
	float GetResolutionHeight();
	int GetDIPCount();
	int GetRenderedTriangles() const {
		return mRenderedTriangleCount;
	}
	void ChangeVideomode(int width, int height, bool fullscreen, bool vsync);
	bool IsAnisotropicFilteringEnabled();
	void SetAnisotropicTextureFiltration(bool state);
	void SetGenericSamplersFiltration(D3DTEXTUREFILTERTYPE filter, bool disableMips);
	void SetSpotLightShadowMapSize(int size);
	void SetPointLightShadowsEnabled(bool state);
	void SetSpotLightShadowsEnabled(bool state);
	bool IsPointLightShadowsEnabled();
	bool IsSpotLightShadowsEnabled();
	ruVector3 GetAmbientColor();
	void SetAmbientColor(ruVector3 ambColor);
	void SetCursor(shared_ptr<ruTexture> texture, int w, int h);
	void SetCursorVisible(bool state);
	void Shutdown();
	shared_ptr<Cursor> & GetCursor();
	int GetTextureChangeCount();
	int GetShaderChangeCount();
	string GetTextureStoragePath();
	void SetTextureStoragePath(const string & path);
	float GetGUIWidthScaleFactor() const;
	float GetGUIHeightScaleFactor() const;
	// Effects control methods
	bool IsFXAAEnabled();
	void SetFXAAEnabled(bool state);
	bool IsHDREnabled();
	void SetHDREnabled(bool state);
	bool IsParallaxEnabled();
	void SetParallaxEnabled(bool state);
	void SetSSAOEnabled(bool state) {
		mSSAOEnabled = state;
	}
	bool IsSSAOEnabled() const {
		return mSSAOEnabled;
	}
	void LoadColorGradingMap(const char * fileName);
	void AddMesh(const shared_ptr<Mesh> & mesh);
};
