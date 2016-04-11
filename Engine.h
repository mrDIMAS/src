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


#include "Timer.h"
#include "BitmapFont.h"
#include "Mesh.h"
#include "Shader.h"

class ParticleSystemRenderer;
class TextRenderer;
class GUIRenderer;
class Cursor;

class Videomode {
public:
	int mWidth;
	int mHeight;
	int mRefreshRate;

	Videomode( int width, int height, int refreshRate ) {
		mWidth = width;
		mHeight = height;
		mRefreshRate = refreshRate;
	}
};

typedef unordered_map< IDirect3DTexture9*, vector<weak_ptr<Mesh>>> MeshMap;

class Engine {
private:
	HWND mWindowHandle;

	shared_ptr<ParticleSystemRenderer> mpParticleSystemRenderer;
	shared_ptr<GUIRenderer> mpGUIRenderer;
	shared_ptr<TextRenderer> mpTextRenderer;
	bool mUsePointLightShadows;
	bool mUseSpotLightShadows;
	bool mRunning;	
	bool mAnisotropicFiltering;
	float mResWidth;
	float mResHeight;
	int mDIPCount;
	D3DPRESENT_PARAMETERS mPresentParameters;
	int mTextureChangeCount;
	string mTextureStoragePath;
	ruVector3 mAmbientColor;
	
	Engine( const Engine & other );
	void operator = ( const Engine & other );
	void UpdateMessagePump( );

	void CreatePhysics( );
	int CreateRenderWindow( int width, int height, int fullscreen );	
	bool mPaused;
	bool mChangeVideomode;
	int mNativeResolutionWidth;
	int mNativeResolutionHeight;
	void SetDefaults();
	vector<Videomode> mVideomodeList;
	
	shared_ptr<Cursor> mCursor;

	MeshMap mDeferredMeshMap;
	MeshMap mForwardMeshMap;

	void PrepareMeshMaps();
	void RenderMesh( const shared_ptr<Mesh> & mesh );

	//**************************************
	// Forward Rendering Stuff
	shared_ptr<PixelShader> mTransparentPixelShader;
	shared_ptr<VertexShader> mTransparentVertexShader;

	//**************************************
	// Deferred Rendering Stuff

	unique_ptr<PixelShader> mAmbientPixelShader;
	unique_ptr<PixelShader> mFXAAPixelShader;

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

	// Standard GBuffer shader
	unique_ptr<VertexShader> mGBufferVertexShader;
	unique_ptr<PixelShader> mGBufferPixelShader;

	// Standard GBuffer shader with skinning
	unique_ptr<VertexShader> mGBufferVertexShaderSkin;
	unique_ptr<PixelShader> mGBufferPixelShaderSkin;

	// Parallax occlusion mapping shaders
	unique_ptr<VertexShader> mGBufferVertexShaderPOM;
	unique_ptr<PixelShader> mGBufferPixelShaderPOM;

	// HDR Stuff
	static const int mHDRDownSampleCount = 6;

	unique_ptr<PixelShader> mToneMapShader;
	unique_ptr<PixelShader> mDownScalePixelShader;
	unique_ptr<PixelShader> mAdaptationPixelShader;
	unique_ptr<PixelShader> mScaleScenePixelShader;
	COMPtr<IDirect3DTexture9> mScaledScene;
	COMPtr<IDirect3DSurface9> mScaledSceneSurf;
	COMPtr<IDirect3DTexture9> mDownSampTex[ mHDRDownSampleCount ];
	COMPtr<IDirect3DSurface9> mDownSampSurf[ mHDRDownSampleCount ];
	COMPtr<IDirect3DTexture9> mAdaptedLuminanceLast;
	COMPtr<IDirect3DTexture9> mAdaptedLuminanceCurrent;

	// Skybox shaders
	unique_ptr<VertexShader> mSkyboxVertexShader;
	unique_ptr<PixelShader> mSkyboxPixelShader;

	// Point light pixel shaders
	unique_ptr<PixelShader> mPointLightPixelShader;
	unique_ptr<PixelShader> mPointLightPixelShaderTexProj;

	// Spot light pixel shaders
	unique_ptr<PixelShader> mSpotLightPixelShader;
	unique_ptr<PixelShader> mSpotLightPixelShaderShadows;

	// Spot light shadow map
	COMPtr<IDirect3DTexture9> mShadowMap;
	COMPtr<IDirect3DSurface9> mShadowMapSurface;
	COMPtr<IDirect3DSurface9> mDefaultDepthStencil;
	COMPtr<IDirect3DSurface9> mDepthStencilSurface;
	unique_ptr<PixelShader> mSpotLightShadowMapPixelShader;
	unique_ptr<VertexShader> mSpotLightShadowMapVertexShader;

	// Quad
	D3DXMATRIX mOrthoProjectionMatrix;
	COMPtr<IDirect3DVertexBuffer9> mQuadVertexBuffer;
	unique_ptr<VertexShader> mQuadVertexShader;

	void RenderFullscreenQuad() {
		mQuadVertexShader->Bind();
		pD3D->SetVertexShaderConstantF( 0, &mOrthoProjectionMatrix.m[0][0], 4 );		
		pD3D->SetStreamSource( 0, mQuadVertexBuffer, 0, sizeof( Vertex ));
		
		pD3D->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
	}

	// Common vertex declaration
	COMPtr<IDirect3DVertexDeclaration9> msVertexDeclaration;

	// D3D9
	COMPtr<IDirect3D9> mpDirect3D;
	COMPtr<IDirect3DDevice9> mpDevice;
	COMPtr<IDirect3DSurface9> mpBackBuffer;	
		
	bool mHDREnabled;
	bool mParallaxEnabled;
	bool mFXAAEnabled;
public:
	explicit Engine();
	virtual ~Engine();
	virtual void OnLostDevice();
	virtual void OnResetDevice();	
	void Initialize( int width, int height, int fullscreen, char vSync );	
    void RenderWorld( );
	void Reset();
	bool IsTextureFormatOk( D3DFORMAT TextureFormat );
	void RegisterDIP();
	float GetResolutionWidth();
	float GetResolutionHeight();
	int GetDIPCount();
	void ChangeVideomode( int width, int height, bool fullscreen, bool vsync );
	bool IsAnisotropicFilteringEnabled();
	void SetAnisotropicTextureFiltration( bool state );
	void SetGenericSamplersFiltration( D3DTEXTUREFILTERTYPE filter, bool disableMips );
	void SetSpotLightShadowMapSize( int size );
	shared_ptr<TextRenderer> & GetTextRenderer();
	void SetPointLightShadowsEnabled( bool state );
	void SetSpotLightShadowsEnabled( bool state );
	bool IsPointLightShadowsEnabled();
	bool IsSpotLightShadowsEnabled();
	ruVector3 GetAmbientColor();
	void SetAmbientColor( ruVector3 ambColor );
	void SetCursor( shared_ptr<ruTexture> texture, int w, int h );
	void SetCursorVisible( bool state );
	bool IsNonPowerOfTwoTexturesSupport();
	void Shutdown();
	shared_ptr<Cursor> & GetCursor();
	int GetTextureChangeCount();
	string GetTextureStoragePath();
	void SetTextureStoragePath( const string & path );
	float GetGUIWidthScaleFactor( ) const;
	float GetGUIHeightScaleFactor( ) const;
	// Effects control methods
	bool IsFXAAEnabled();
	void SetFXAAEnabled( bool state );
	bool IsHDREnabled();
	void SetHDREnabled( bool state );
	bool IsParallaxEnabled();
	void SetParallaxEnabled( bool state );
	void AddMesh( const shared_ptr<Mesh> & mesh );
};
