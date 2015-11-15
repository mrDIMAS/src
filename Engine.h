#pragma once


#include "Timer.h"
#include "BitmapFont.h"
#include "Mesh.h"
#include "FPSCounter.h"

class DeferredRenderer;
class ForwardRenderer;
class ParticleSystemRenderer;
class TextRenderer;
class GUIRenderer;

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

class Engine {
private:
	HWND mWindowHandle;
	IDirect3D9 * mpDirect3D;
	IDirect3DDevice9 * mpDevice;
	IDirect3DSurface9 * mpBackBuffer;
	DeferredRenderer * mpDeferredRenderer;
	IDirect3DTexture9 * mpWhiteTexture;
	ParticleSystemRenderer * mpParticleSystemRenderer;
	ForwardRenderer * mpForwardRenderer;
	GUIRenderer * mpGUIRenderer;
	TextRenderer * mpTextRenderer;
	FPSCounter mFPSCounter;
	bool mUsePointLightShadows;
	bool mUseSpotLightShadows;
	bool mHDREnabled;
	bool mRunning;
	bool mFXAAEnabled;
	bool mAnisotropicFiltering;
	float mResWidth;
	float mResHeight;
	int mDIPCount;
	D3DPRESENT_PARAMETERS mPresentParameters;
	int mTextureChangeCount;
	string mTextureStoragePath;
	ruVector3 mAmbientColor;
	explicit Engine();
	Engine( const Engine & other );
	void operator = ( const Engine & other );
	static LRESULT CALLBACK WindowProcess ( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam );
	void UpdateMessagePump( );
	void RenderMeshesIntoGBuffer();
	void CreatePhysics( );
	int CreateRenderWindow( int width, int height, int fullscreen );	
	bool mPaused;
	bool mChangeVideomode;
	int mNativeResolutionWidth;
	int mNativeResolutionHeight;
	void SetDefaults();
	vector<Videomode> mVideomodeList;
public:
	virtual void OnLostDevice();
	virtual void OnResetDevice();
	static Engine & Instance();	
	void Initialize( int width, int height, int fullscreen, char vSync );
	virtual ~Engine();
    void RenderWorld( );
    void SetVertexShaderMatrix( UINT startRegister, D3DMATRIX * matrix );
    void SetVertexShaderFloat3( UINT startRegister, float * v );
    void SetVertexShaderFloat( UINT startRegister, float v );
    void SetVertexShaderInt( UINT startRegister, int v );
    void SetPixelShaderMatrix( UINT startRegister, D3DMATRIX * matrix );
    void SetPixelShaderFloat3( UINT startRegister, float * v );
    void SetPixelShaderFloat( UINT startRegister, float v );
    void SetPixelShaderInt( UINT startRegister, int v );
	void SetVertexShaderVector3( UINT startRegister, ruVector3 v );
	void Reset();
	int GetDeviceRefCount();
	void Pause();
	void Continue();
	IDirect3DDevice9 * GetDevice();
	bool IsTextureFormatOk( D3DFORMAT TextureFormat );
	void RegisterDIP();
	float GetResolutionWidth();
	float GetResolutionHeight();
	int GetDIPCount();
	void ChangeVideomode( int width, int height, bool fullscreen, bool vsync );
	bool IsAnisotropicFilteringEnabled();
	void SetAnisotropicTextureFiltration( bool state );
	void SetDiffuseNormalSamplersFiltration( D3DTEXTUREFILTERTYPE filter, bool disableMips );
	void SetSpotLightShadowMapSize( int size );
	DeferredRenderer * GetDeferredRenderer();
	ForwardRenderer * GetForwardRenderer();
	TextRenderer * GetTextRenderer();
	void SetPointLightShadowsEnabled( bool state );
	void SetSpotLightShadowsEnabled( bool state );
	bool IsPointLightShadowsEnabled();
	bool IsSpotLightShadowsEnabled();
	ruVector3 GetAmbientColor();
	void SetAmbientColor( ruVector3 ambColor );
	bool IsHDREnabled();
	void SetHDREnabled( bool state );
	bool IsNonPowerOfTwoTexturesSupport();
	void Shutdown();
	bool IsFXAAEnabled();
	void SetFXAAEnabled( bool state );
	int GetTextureChangeCount();
	string GetTextureStoragePath();
	IDirect3DSurface9 * GetBackBuffer();
	void SetTextureStoragePath( const string & path );
	void DrawIndexedTriangleList( int vertexCount, int faceCount );
};
