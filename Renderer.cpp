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

#include "Precompiled.h"
#include "Light.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Renderer.h"
#include "Physics.h"
#include "BitmapFont.h"
#include "SceneFactory.h"
#include "Cursor.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "ParticleSystem.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Camera.h"
#include "Skybox.h"
#include "Utility.h"
#include "Renderer.h"
#include "SceneFactory.h"
#include "BitmapFont.h"
#include "GUIRect.h"
#include "GUIText.h"
#include "GUIButton.h"
#include "GUIFactory.h"
#include <random>

IDirect3DDevice9 * pD3D;
unique_ptr<Renderer> pEngine;
Mouse mouse;
Keyboard keyboard;

struct A8R8G8B8Pixel {
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;

	A8R8G8B8Pixel( uint8_t _a, uint8_t _r, uint8_t _g, uint8_t _b ) : a( _a ), r( _r ), g( _g ), b( _b ) { };
};

struct XYZNormalVertex {
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
};

struct CameraDirection {
	ruVector3 target;
	ruVector3 up;
};

struct GPUFloatRegister {
	float a, b, c, d;

	void Set( float v1 ) {
		a = v1;
		b = 0.0f;
		c = 0.0f;
		d = 0.0f;
	}

	void Set( float v1, float v2 ) {
		a = v1;
		b = v2;
		c = 0.0f;
		d = 0.0f;
	}

	void Set( float v1, float v2, float v3 ) {
		a = v1;
		b = v2;
		c = v3;
		d = 0.0f;
	}

	void Set( float v1, float v2, float v3, float v4 ) {
		a = v1;
		b = v2;
		c = v3;
		d = v4;
	}
};

class GPUBoolRegisterStack {
public:
	BOOL mBooleans[8];
	int mBooleanCount;

	GPUBoolRegisterStack() : mBooleanCount( 0 ) {
		
	}

	void Clear() {
		mBooleanCount = 0;
	}

	void Push( BOOL boolean ) {
		mBooleans[mBooleanCount++] = boolean;
	}

	const BOOL * GetPointer( ) const {
		return &mBooleans[0];
	}
};

class GPUFloatRegisterStack {
public:
	GPUFloatRegister mRegisters[256];
	int mRegisterCount;

	GPUFloatRegisterStack() : mRegisterCount( 0 ) {

	}

	void Clear( ) {
		mRegisterCount = 0;
	}

	void PushMatrix( const D3DXMATRIX & m ) {
		mRegisters[mRegisterCount++].Set( m._11, m._12, m._13, m._14 );
		mRegisters[mRegisterCount++].Set( m._21, m._22, m._23, m._24 );
		mRegisters[mRegisterCount++].Set( m._31, m._32, m._33, m._34 );
		mRegisters[mRegisterCount++].Set( m._41, m._42, m._43, m._44 );
	}

	void PushIdentityMatrix() {
		mRegisters[mRegisterCount++].Set( 1, 0, 0, 0 );
		mRegisters[mRegisterCount++].Set( 0, 1, 0, 0 );
		mRegisters[mRegisterCount++].Set( 0, 0, 1, 0 );
		mRegisters[mRegisterCount++].Set( 0, 0, 0, 1 );
	}

	void PushVector( const ruVector3 & v ) {
		mRegisters[mRegisterCount++].Set( v.x, v.y, v.z );
	}

	void PushVector( const ruVector2 & v ) {
		mRegisters[mRegisterCount++].Set( v.x, v.y );
	}

	void PushFloat( float v1 ) {
		mRegisters[mRegisterCount++].Set( v1 );
	}

	void PushFloat( float v1, float v2 ) {
		mRegisters[mRegisterCount++].Set( v1, v2 );
	}

	void PushFloat( float v1, float v2, float v3 ) {
		mRegisters[mRegisterCount++].Set( v1, v2, v3 );
	}

	void PushFloat( float v1, float v2, float v3, float v4 ) {
		mRegisters[mRegisterCount++].Set( v1, v2, v3, v4 );
	}

	const float * GetPointer( ) const {
		return &mRegisters[0].a;
	}
};

/*
=========
SetUniformScaleTranslationMatrix
=========
*/
D3DXMATRIX SetUniformScaleTranslationMatrix( float s, const ruVector3 & p ) {
	return D3DXMATRIX ( 
		s, 0.0f, 0.0f, 0.0f,
		0.0f, s, 0.0f, 0.0f,
		0.0f, 0.0f, s, 0.0f,
		p.x, p.y, p.z, 1.0f 
	);
}

/*
=========
WindowProcess
=========
*/
LRESULT CALLBACK WindowProcess( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch ( msg ) {
	case WM_DESTROY:
		ruEngine::Free();
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		keyboard.KeyDown( (lParam & 0x01FF0000) >> 16 );
		break;
	case WM_KEYUP: 
		keyboard.KeyUp( (lParam & 0x01FF0000) >> 16 );
		break;	
	case WM_MOUSEMOVE:
		mouse.Move( LOWORD( lParam ), HIWORD( lParam ));
		break;
	case WM_LBUTTONDOWN:
		mouse.ButtonDown( ruInput::MouseButton::Left );
		break;
	case WM_RBUTTONDOWN:
		mouse.ButtonDown( ruInput::MouseButton::Right );
		break;
	case WM_MBUTTONDOWN:
		mouse.ButtonDown( ruInput::MouseButton::Middle );
		break;
	case WM_LBUTTONUP:
		mouse.ButtonUp( ruInput::MouseButton::Left );
		break;
	case WM_RBUTTONUP:
		mouse.ButtonUp( ruInput::MouseButton::Right );
		break;
	case WM_MBUTTONUP:
		mouse.ButtonUp( ruInput::MouseButton::Middle );
		break;
	case WM_MOUSEWHEEL:
		mouse.Wheel( ((short)HIWORD( wParam )) / WHEEL_DELTA );
		break;
	}	
	return DefWindowProc ( wnd, msg, wParam, lParam );
}

/*
=========
Renderer
=========
*/
Renderer::Renderer( int width, int height, int fullscreen, char vSync ) :  
	mAmbientColor( 0.05, 0.05, 0.05 ), 
	mUsePointLightShadows( false ),
	mUseSpotLightShadows( false ), 
	mHDREnabled( false ), 
	mRunning( true ), 
	mFXAAEnabled( false ),
	mTextureStoragePath( "data/textures/generic/" ), 
	mParallaxEnabled( true ) ,
	mPaused( false ),
	mChangeVideomode( false ),
	mShadersChangeCount( 0 ),
	mRenderedTriangleCount( 0 ),
	mTextureChangeCount( 0 )
{
	if ( width == 0 || height == 0 ) {
        width = GetSystemMetrics ( SM_CXSCREEN );
		height = GetSystemMetrics ( SM_CYSCREEN );
		Log::Write( "Zero passed as a resolution, using desktop native resolution." );
    }
  
    // Create Render window
	const wchar_t * className = L"The Mine";
	WNDCLASSEXW wcx = { 0 };
	wcx.cbSize = sizeof ( wcx );
	wcx.hCursor = LoadCursor ( NULL, IDC_ARROW );
	wcx.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
	wcx.hInstance = GetModuleHandle ( 0 );
	wcx.lpfnWndProc = WindowProcess;
	wcx.lpszClassName = className;
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassExW ( &wcx );

	DWORD style = fullscreen ? ( style = WS_POPUP ) : ( WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS );

	RECT wRect = { 0, 0, width, height };

	AdjustWindowRect ( &wRect, style, 0 );

	mWindowHandle = CreateWindowW( className, className, style, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, GetModuleHandle ( 0 ), 0 );

	// Setup window
	ShowWindow ( mWindowHandle, SW_SHOW );
	UpdateWindow ( mWindowHandle );
	SetActiveWindow ( mWindowHandle );
	SetForegroundWindow ( mWindowHandle );

	ruInput::Init( mWindowHandle );

    // Create Direct3D9
    mpDirect3D.Set( Direct3DCreate9( D3D_SDK_VERSION ));

    if( !mpDirect3D ) {
		Log::Error( "Failed to Direct3DCreate9! Ensure, that you have latest video drivers! Engine initialization failed!" );
    } else {
		Log::Write( "Direct3D 9 Created successfully!" );
	}

    // Check if Pixel Shader 3.0+ is supported
    D3DCAPS9 dCaps;
    mpDirect3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &dCaps );

    if( D3DSHADER_VERSION_MAJOR( dCaps.PixelShaderVersion ) < 3 ) {
		mpDirect3D->Release();
        Log::Error( "Your graphics card doesn't support Pixel Shader 3.0. Engine initialization failed! Buy a modern video card!" );        
    }

	// Check passed resolution
	bool passedResolutionValid = false;
	for( int i = 0; i < mpDirect3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8 ); i++ ) {
		D3DDISPLAYMODE mode;
		mpDirect3D->EnumAdapterModes( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &mode );
		Log::Write( StringBuilder( "Videomode: " ) << mode.Width << " x " << mode.Height << " x 32 @ " << mode.RefreshRate );
		mVideomodeList.push_back( Videomode( mode.Width, mode.Height, mode.RefreshRate ));
		if( mode.Width == width && mode.Height == height ) {
			passedResolutionValid = true;
		}
	}

	if( passedResolutionValid ) {
		Log::Write( StringBuilder( "Setting resolution " ) << width << " x " << height << "..." );
	} else {
		Log::Write( StringBuilder( "WARNING! Resolution " ) << width << " x " << height << " is invalid! Using native desktop resolution instead" );
		width = GetSystemMetrics ( SM_CXSCREEN );
		height = GetSystemMetrics ( SM_CYSCREEN );
	}

    mResWidth = width;
    mResHeight = height;

	mNativeResolutionWidth = GetSystemMetrics ( SM_CXSCREEN );
	mNativeResolutionHeight = GetSystemMetrics ( SM_CYSCREEN );

    D3DDISPLAYMODE displayMode = { 0 };
    mpDirect3D->GetAdapterDisplayMode ( D3DADAPTER_DEFAULT, &displayMode );

    // present parameters
    memset( &mPresentParameters, 0, sizeof( mPresentParameters ));

    mPresentParameters.BackBufferCount = 2;
    mPresentParameters.EnableAutoDepthStencil = TRUE;
    if( vSync ) {
        mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    } else {
        mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    mPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
    mPresentParameters.BackBufferWidth = width;
    mPresentParameters.BackBufferHeight = height;
	mPresentParameters.hDeviceWindow = mWindowHandle;
    if ( fullscreen ) {
        mPresentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
        mPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        mPresentParameters.Windowed = FALSE;
        mPresentParameters.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
    } else {
        mPresentParameters.BackBufferFormat = displayMode.Format;
        mPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        mPresentParameters.Windowed = TRUE;
    }

    // No multisampling, because of deferred shading
    mPresentParameters.MultiSampleQuality = 0;
	mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;

    // Create device
    if( FAILED( mpDirect3D->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mWindowHandle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &mPresentParameters, &mpDevice ))) {
		mpDirect3D->Release();
		Log::Error( "Engine initialization failed! Buy a modern video card!" );	
	} else {
		Log::Write( "Direct3D 9 Device Created successfully!" );
	}

	pD3D = mpDevice;

	pD3D->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &mpBackBuffer );

	// Create common vertex declaration
	D3DVERTEXELEMENT9 commonVertexDeclaration[ ] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 44, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 }, // bone indices
		{ 0, 60, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 }, // bone weights
		D3DDECL_END()
	};

	pD3D->CreateVertexDeclaration( commonVertexDeclaration, &msVertexDeclaration );
	
    SetDefaults();

	// Initialize physics
    Physics::CreateWorld();

	// Initialize sound sub-system
	pfSystemCreateLogFile( "ProjectF.log" ); 

	pfSystemEnableMessagesOutputToConsole(); 
	pfSystemEnableMessagesOutputToLogFile(); 

    pfSystemInit( );
    pfSetListenerDopplerFactor( 0 );
	

	//***************************************************************
	// Init Deferred Rendering Stuff

	// Create render targets 
	pD3D->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &mHDRFrame, nullptr );
	mHDRFrame->GetSurfaceLevel( 0, &mHDRFrameSurface );

	// Create additional render targets for posteffects
	for( int i = 0; i < 2; i++ ) {
		pD3D->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mFrame[i], nullptr );
		mFrame[i]->GetSurfaceLevel( 0, &mFrameSurface[i] );
	}

	// FXAA Pixel Shader
	LoadPixelShader( mFXAAPixelShader, "data/shaders/fxaa.pso" );

	// Create G-Buffer ( Depth: R32F, Diffuse: ARGB8, Normal: ARGB8 )
	pD3D->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mDepthMap, nullptr );
	pD3D->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mNormalMap, nullptr );
	pD3D->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mDiffuseMap, nullptr );

	mDepthMap->GetSurfaceLevel( 0, &mDepthSurface );
	mNormalMap->GetSurfaceLevel( 0, &mNormalSurface );
	mDiffuseMap->GetSurfaceLevel( 0, &mDiffuseSurface );

	// Ambient light pixel shader
	LoadPixelShader( mAmbientPixelShader, "data/shaders/deferredAmbientLight.pso" );

	// GBuffer shaders
	LoadVertexShader( mGBufferVertexShader, "data/shaders/deferredGBuffer.vso" );
	LoadPixelShader( mGBufferPixelShader, "data/shaders/deferredGBuffer.pso" );
	LoadPixelShader( mDeferredBlending, "data/shaders/deferredBlending.pso" );
	LoadPixelShader( mDeferredLightShader, "data/shaders/deferredLighting.pso" ); 

	// Init HDR Stuff
	int scaledSize = IntegerPow( 2, mHDRDownSampleCount + 1 );
	pD3D->CreateTexture( scaledSize, scaledSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mScaledScene, nullptr );
	mScaledScene->GetSurfaceLevel( 0, &mScaledSceneSurf );

	for( int i = 0; i < mHDRDownSampleCount; i++ ) {
		int size = IntegerPow( 2, mHDRDownSampleCount - i );
		pD3D->CreateTexture( size, size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mDownSampTex[ i ], nullptr );
	}

	pD3D->CreateTexture( 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mAdaptedLuminanceLast, nullptr );
	pD3D->CreateTexture( 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mAdaptedLuminanceCurrent, nullptr );

	for( int i = 0; i < mHDRDownSampleCount; i++ ) {
		mDownSampTex[i]->GetSurfaceLevel( 0, &mDownSampSurf[i] );
	}

	LoadPixelShader( mToneMapShader, "data/shaders/hdrTonemap.pso" );
	LoadPixelShader( mScaleScenePixelShader, "data/shaders/hdrScale.pso" );
	LoadPixelShader( mAdaptationPixelShader, "data/shaders/hdrAdaptation.pso" );
	LoadPixelShader( mDownScalePixelShader, "data/shaders/hdrDownscale.pso" );

	// Skybox shader
	LoadVertexShader( mSkyboxVertexShader, "data/shaders/skybox.vso" );
	LoadPixelShader( mSkyboxPixelShader, "data/shaders/skybox.pso" );

	// Spot light shadow map
	int shadowMapWidth = width * 0.5f;
	int shadowMapHeight = height * 0.5f;

	LoadVertexShader( mShadowMapVertexShader, "data/shaders/shadowMap.vso" );
	LoadPixelShader( mShadowMapPixelShader, "data/shaders/shadowMap.pso" );

	pD3D->CreateTexture( shadowMapWidth, shadowMapHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mShadowMap, nullptr );
	mShadowMap->GetSurfaceLevel( 0, &mShadowMapSurface );

	pD3D->GetDepthStencilSurface( &mDefaultDepthStencil );
	pD3D->CreateDepthStencilSurface( shadowMapWidth, shadowMapHeight, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &mDepthStencilSurface, 0 );

	// Particle shaders
	LoadVertexShader( mParticleSystemVertexShader, "data/shaders/particle.vso" );
	LoadPixelShader( mParticleSystemPixelShader, "data/shaders/particle.pso" );

	// Create Bounding volumes
	int quality = 6;

	D3DVERTEXELEMENT9 vd[ ] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	XYZNormalVertex * data;

	D3DXCreateSphere( pD3D, 1.0, quality, quality, &mBoundingSphere, 0 );
	mBoundingSphere->UpdateSemantics( vd );

	D3DXCreateCylinder( pD3D, 0.0f, 1.0f, 1.0f, quality, quality, &mBoundingCone, 0 );

	mBoundingCone->LockVertexBuffer( 0, reinterpret_cast<void**>( &data ));
	D3DXMATRIX tran, rot90, transform;
	D3DXMatrixTranslation( &tran, 0, -0.5, 0 );
	D3DXMatrixRotationAxis( &rot90, &D3DXVECTOR3( 1, 0, 0 ), SIMD_HALF_PI ); 
	transform = rot90 * tran;
	for( int i = 0; i < mBoundingCone->GetNumVertices(); i++ ) {
		XYZNormalVertex * v = &data[ i ];
		D3DXVec3TransformCoord( &v->p, &v->p, &transform );
	}
	mBoundingCone->UnlockVertexBuffer();
	mBoundingCone->UpdateSemantics( vd );

	D3DXCreateSphere( pD3D, 1.0, quality, quality, &mBoundingStar, 0 );

	mBoundingStar->LockVertexBuffer( 0, reinterpret_cast<void**>( &data ));
	int n = 0;
	for( int i = 0; i < mBoundingStar->GetNumVertices(); i++ ) {
		XYZNormalVertex * v = &data[ i ];
		n++;
		if( n == 5 ) {
			v->p.x = 0;
			v->p.y = 0;
			v->p.z = 0;
			n = 0;
		}
	}
	mBoundingStar->UnlockVertexBuffer();
	mBoundingStar->UpdateSemantics( vd );

	

	// Create fullscreen quad
	LoadVertexShader( mQuadVertexShader, "data/shaders/quad.vso" );

	Vertex vertices[6];
	vertices[0] = Vertex( ruVector3( -0.5, -0.5, 0.0f ), ruVector2( 0, 0 ));
	vertices[1] = Vertex( ruVector3( GetResolutionWidth() - 0.5, -0.5, 0.0f ), ruVector2( 1, 0 ));
	vertices[2] = Vertex( ruVector3( -0.5, GetResolutionHeight() - 0.5, 0 ), ruVector2( 0, 1 ));
	vertices[3] = Vertex( ruVector3( GetResolutionWidth() - 0.5, -0.5, 0.0f ), ruVector2( 1, 0 ));
	vertices[4] = Vertex( ruVector3( GetResolutionWidth() - 0.5, GetResolutionHeight() - 0.5, 0.0f ), ruVector2( 1, 1 ));
	vertices[5] = Vertex( ruVector3( -0.5, GetResolutionHeight() - 0.5, 0.0f ), ruVector2( 0, 1 ));

	void * lockedData;
	pD3D->CreateVertexBuffer( 6 * sizeof( Vertex ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &mQuadVertexBuffer, 0 );
	mQuadVertexBuffer->Lock( 0, 0, &lockedData, 0 );
	memcpy( lockedData, vertices, sizeof( Vertex ) * 6 );
	mQuadVertexBuffer->Unlock( );

	D3DXMatrixOrthoOffCenterLH ( &mOrthoProjectionMatrix, 0, GetResolutionWidth(), GetResolutionHeight(), 0, 0, 1024 );

	// Skybox
	float size = 1024.0f;
	Vertex fv[] = {
		Vertex( ruVector3( -size,  size, -size ), ruVector2( 0.0f, 0.0f )),
		Vertex( ruVector3( size,  size, -size ), ruVector2( 1.0f, 0.0f  )),
		Vertex( ruVector3( size, -size, -size ), ruVector2( 1.0f, 1.0f )),
		Vertex( ruVector3( -size, -size, -size ), ruVector2( 0.0f, 1.0f )),
		Vertex( ruVector3( size,  size,  size ), ruVector2( 0.0f, 0.0f )),
		Vertex( ruVector3( -size,  size,  size ), ruVector2( 1.0f, 0.0f )),
		Vertex( ruVector3( -size, -size,  size ), ruVector2( 1.0f, 1.0f )),
		Vertex( ruVector3( size, -size,  size ), ruVector2( 0.0f, 1.0f )),
		Vertex( ruVector3( size,  size, -size ), ruVector2( 0.0f, 0.0f )),
		Vertex( ruVector3( size,  size,  size ), ruVector2( 1.0f, 0.0f )),
		Vertex( ruVector3( size, -size,  size ), ruVector2( 1.0f, 1.0f )),
		Vertex( ruVector3( size, -size, -size ), ruVector2( 0.0f, 1.0f )),
		Vertex( ruVector3( -size,  size,  size ), ruVector2( 0.0f, 0.0f )),
		Vertex( ruVector3( -size,  size, -size ), ruVector2( 1.0f, 0.0f )),
		Vertex( ruVector3( -size, -size, -size ), ruVector2( 1.0f, 1.0f )),
		Vertex( ruVector3( -size, -size,  size ), ruVector2( 0.0f, 1.0f )),
		Vertex( ruVector3( -size,  size,  size ), ruVector2( 0.0f, 0.0f )),
		Vertex( ruVector3( size,  size,  size ), ruVector2( 1.0f, 0.0f )),
		Vertex( ruVector3( size,  size, -size ), ruVector2( 1.0f, 1.0f )),
		Vertex( ruVector3( -size,  size, -size ), ruVector2( 0.0f, 1.0f ))
	};

	unsigned short indices [] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19 };

	pD3D->CreateVertexBuffer( sizeof( fv ), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &mSkyboxVertexBuffer, 0 );	
	mSkyboxVertexBuffer->Lock( 0, 0, &lockedData, 0 );
	memcpy( lockedData, fv, sizeof( fv ));
	mSkyboxVertexBuffer->Unlock();

	pD3D->CreateIndexBuffer( sizeof( indices ), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mSkyboxIndexBuffer, 0 );
	mSkyboxIndexBuffer->Lock( 0, 0, &lockedData, 0 );
	memcpy( lockedData, indices, sizeof( indices ) );
	mSkyboxIndexBuffer->Unlock();

	// GUI Stuff
	pD3D->CreateVertexBuffer( 6 * sizeof( Vertex ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ, D3DPOOL_DEFAULT, &mRectVertexBuffer, 0 );

	LoadVertexShader( mGUIVertexShader, "data/shaders/gui.vso" );
	LoadPixelShader( mGUIPixelShader, "data/shaders/gui.pso" );

	mTextMaxChars = 8192;
	int vBufLen = mTextMaxChars * 4 * sizeof( Vertex );
	pD3D->CreateVertexBuffer( vBufLen, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_TEX1 | D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &mTextVertexBuffer, nullptr );
	int iBufLen = mTextMaxChars * 2 * sizeof( Triangle );
	pD3D->CreateIndexBuffer( iBufLen, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mTextIndexBuffer, nullptr );

	// Bloom stuff
	float bloomWidth = width * 0.5f;
	float bloomHeight = height * 0.5f;
	mBloomDX = ruVector2( 1.0f / bloomWidth, 0.0f );
	mBloomDY = ruVector2( 0.0f, 1.0f / bloomHeight );
	pD3D->CreateTexture( bloomWidth, bloomHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &mBloomTexture, nullptr );
	mBloomTexture->GetSurfaceLevel( 0, &mBloomTextureSurface );

	pD3D->CreateTexture( bloomWidth, bloomHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &mBloomBlurredTexture, nullptr );
	mBloomBlurredTexture->GetSurfaceLevel( 0, &mBloomBlurredSurface );
	
	LoadPixelShader( mBloomPixelShader, "data/shaders/bloom.pso" );
	LoadPixelShader( mGaussianBlurShader, "data/shaders/gaussianblur.pso" );

	// Create default textures
	D3DLOCKED_RECT lockedRect;	
	A8R8G8B8Pixel * pixels;

	IDirect3DSurface9 * surface;
	const int defTexSize = 4;

	// Create default point light projection cube map
	pD3D->CreateCubeTexture( defTexSize, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &mWhiteCubeMap, nullptr );
	for( int face = 0; face < 6; ++face ) {
		mWhiteCubeMap->LockRect( (D3DCUBEMAP_FACES)face, 0, &lockedRect, nullptr, 0 );
		pixels = (A8R8G8B8Pixel*)lockedRect.pBits;
		for( int i = 0; i < defTexSize * defTexSize; ++i ) {
			(*pixels++) = A8R8G8B8Pixel( 255, 255, 255, 255 );
		}
		mWhiteCubeMap->UnlockRect( (D3DCUBEMAP_FACES)face, 0 );
	}
	
	// Create default normal map
	pD3D->CreateTexture( defTexSize, defTexSize, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &mDefaultNormalMap, nullptr );
	mDefaultNormalMap->GetSurfaceLevel( 0, &surface );
	surface->LockRect( &lockedRect, nullptr, 0 );
	pixels = (A8R8G8B8Pixel*)lockedRect.pBits;
	for( int i = 0; i < defTexSize * defTexSize; ++i ) {
		(*pixels++) = A8R8G8B8Pixel( 255, 128, 128, 255 );
	}
	surface->UnlockRect();
	surface->Release();

	// Create default white texture
	pD3D->CreateTexture( defTexSize, defTexSize, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &mWhiteMap, nullptr );
	mWhiteMap->GetSurfaceLevel( 0, &surface );
	surface->LockRect( &lockedRect, nullptr, 0 );
	pixels = (A8R8G8B8Pixel*)lockedRect.pBits;
	for( int i = 0; i < defTexSize * defTexSize; ++i ) {
		(*pixels++) = A8R8G8B8Pixel( 255, 255, 255, 255 );
	}
	surface->UnlockRect();
	surface->Release();

	// Cube shadowmap
	pD3D->CreateCubeTexture( 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mCubeShadowMap, nullptr );
}

/*
=========
~Engine
=========
*/
Renderer::~Renderer() {	
    while( Timer::msTimerList.size() ) {
        delete Timer::msTimerList.front();
    }
    for( auto & kv : CubeTexture::all ) {
        delete kv.second;
    }
    Physics::DestructWorld();
}



/*
=========
RenderWorld
	- All rendering is done here
	- Firstly done deferred rendering
	- Then goes rendering of transparent meshes, particles and etc. 
	- And finally GUI is rendered
	- Actually this method is pretty straigthforward and clear
=========
*/
void Renderer::RenderWorld() {	
	// Dispatch all window messages
	MSG message;
	while ( PeekMessage ( &message, NULL, 0, 0, PM_REMOVE ) ) {
		DispatchMessage ( &message );
		if ( message.message == WM_QUIT ) {
			Shutdown();
		}
	}

	// Check for lost device (freaking D3D9)
	if( pD3D->TestCooperativeLevel() == D3DERR_DEVICELOST ) {
		if( !mPaused ) {
			Log::Write( "Device lost. Engine paused!" );
		}
		mPaused = true;		
	}
	
	if( mPaused ) {
		if( pD3D->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )  {
			OnLostDevice();
			mPaused = false;
			Log::Write( "Lost device handled. Engine restored!" );
		}
		return;
	}

	// Rendering could not be done when there is no camera
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();

	if( !camera || !mRunning ) {
        return;
    }
   
	// Remove unreferenced nodes from mesh list and remove list if it is empty
	// This is necessary to be sure that weak_ptr<>.lock() returns valid pointer
	for( auto texGroupPairIter = mDeferredMeshMap.begin(); texGroupPairIter != mDeferredMeshMap.end();  ) {
		auto & texGroupPair = *texGroupPairIter;
		if( texGroupPair.second.size() ) {
			for( auto & meshIter = texGroupPair.second.begin(); meshIter != texGroupPair.second.end(); ) {
				if( (*meshIter).use_count() ) {
					++meshIter;
				} else {
					meshIter = texGroupPair.second.erase( meshIter );
				}
			}
			++texGroupPairIter;
		} else {
			texGroupPairIter = mDeferredMeshMap.erase( texGroupPairIter );
		}
	}

    // Clear statistics
    mDIPCount = 0;
    mTextureChangeCount = 0;
	mShadersChangeCount = 0;
	mRenderedTriangleCount = 0;
	
	// Begin rendering
    pD3D->BeginScene();
	
    //********************************
	// Geometry pass
	//
	// Samplers layout: 
	//	0 - diffuse 
	//	1 - normal
	//	2 - height
	//
	// Render Target layout: 
	//	0 - depth
	//	1 - normal
	//	2 - diffuse
	GPUFloatRegisterStack gpuFloatRegisterStack;
	GPUBoolRegisterStack gpuBoolRegisterStack;

	pD3D->SetRenderState( D3DRS_ZENABLE, TRUE );
	pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	pD3D->SetTexture( 0, 0 );
	pD3D->SetTexture( 1, 0 );
	pD3D->SetTexture( 2, 0 );
	mTextureChangeCount += 3;

	pD3D->SetRenderTarget( 0, mDepthSurface );
	pD3D->SetRenderTarget( 1, mNormalSurface );
	pD3D->SetRenderTarget( 2, mDiffuseSurface );

	pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

	if( mHDREnabled ) {
		pD3D->SetSamplerState( 0, D3DSAMP_SRGBTEXTURE, TRUE );
	}

	pD3D->SetVertexDeclaration( msVertexDeclaration );

	pD3D->SetVertexShader( mGBufferVertexShader );
	pD3D->SetPixelShader( mGBufferPixelShader );

	mShadersChangeCount += 2;

	for( auto & texGroupPair : mDeferredMeshMap ) {
		IDirect3DTexture9 * pDiffuseTexture = texGroupPair.first;
		IDirect3DTexture9 * pNormalTexture = nullptr;

		// skip group, if it has no meshes
		if( texGroupPair.second.size() == 0 ) {
			continue;
		}

		pD3D->SetTexture( 0, pDiffuseTexture );
		++mTextureChangeCount;

		for( auto weakMesh : texGroupPair.second ) {
			shared_ptr<Mesh> & pMesh = weakMesh.lock();

			if( !pMesh->IsHardwareBuffersGood() ) {
				continue;
			}
			if( pMesh->GetVertices().size() == 0 ) {
				continue;
			}

			if( pMesh->mHeightTexture ) {
				pD3D->SetTexture( 2, pMesh->mHeightTexture->GetInterface() );
			}

			// prevent overhead with normal texture
			if( pMesh->mNormalTexture ) {
				if( pMesh->mNormalTexture->GetInterface() != pNormalTexture ) {
					pD3D->SetTexture( 1, pMesh->mNormalTexture->GetInterface());
					++mTextureChangeCount;
					pNormalTexture = pMesh->mNormalTexture->GetInterface();
				}
			} else {
				pD3D->SetTexture( 1, mDefaultNormalMap );
			}

			// render mesh
			auto & owners = pMesh->GetOwners();

			for( auto weakOwner : owners ) {
				shared_ptr<SceneNode> & pOwner = weakOwner.lock();
						
				bool visible = pOwner->IsBone() ? false : pOwner->IsVisible();
							
				if( pOwner->mTwoSidedLighting ) {
					pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				} else {
					pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
				}

				if( visible && ( pOwner->IsInFrustum() || pOwner->IsSkinned() ) ) {
					camera->EnterDepthHack( pOwner->GetDepthHack() );
							
					D3DXMATRIX world;
					if( pMesh->IsSkinned() ) {
						D3DXMatrixIdentity( &world );
					} else {
						world = pOwner->GetWorldMatrix();
					}

					// Load pixel shader constants
					gpuBoolRegisterStack.Clear();
					gpuBoolRegisterStack.Push( (IsParallaxEnabled() && pMesh->mHeightTexture) ? TRUE : FALSE );
					pD3D->SetPixelShaderConstantB( 0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount );

					gpuFloatRegisterStack.Clear();
					gpuFloatRegisterStack.PushFloat( pOwner->GetAlbedo() );
					gpuFloatRegisterStack.PushFloat( pMesh->GetOpacity() );
					pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );
								
					// Load vertex shader constants
					gpuBoolRegisterStack.Clear();
					gpuBoolRegisterStack.Push( pMesh->IsSkinned() ? TRUE : FALSE );
					pD3D->SetVertexShaderConstantB( 0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount );

					gpuFloatRegisterStack.Clear();
					gpuFloatRegisterStack.PushMatrix( world );
					gpuFloatRegisterStack.PushMatrix( world * camera->mViewProjection );
					gpuFloatRegisterStack.PushVector( camera->GetPosition() );
					gpuFloatRegisterStack.PushVector( pOwner->GetTexCoordFlow() );
					for( auto bone : pMesh->GetBones() ) {
						shared_ptr<SceneNode> boneNode = bone->mNode.lock();
						if( boneNode ) {
							gpuFloatRegisterStack.PushMatrix( TransformToMatrix( boneNode->GetRelativeTransform() * pOwner->GetLocalTransform() ));
						}
					}						
					pD3D->SetVertexShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

					RenderMesh( pMesh );

					camera->LeaveDepthHack();						
				}
			}			
		}
	}

	//********************************
	// Lighting pass
	//
	// Samplers layout: 
	//	0 - depth
	//	1 - normal
	//	2 - diffuse
	//
	// Render Target layout: 
	//	0 - backbuffer, hdr buffer or temp color buffer
		
	pD3D->SetTexture( 0, nullptr );
	pD3D->SetTexture( 1, nullptr );
	pD3D->SetTexture( 2, nullptr );
	mTextureChangeCount += 3;

	pD3D->SetSamplerState( 0, D3DSAMP_SRGBTEXTURE, FALSE );

	// select render target 
	IDirect3DSurface9 * renderTarget = mFrameSurface[0];

	// If HDR enabled, render all in high-precision float texture
	if( mHDREnabled ) {
		renderTarget = mHDRFrameSurface;
	}

	pD3D->SetRenderTarget( 0, renderTarget );
	pD3D->SetRenderTarget( 1, nullptr );
	pD3D->SetRenderTarget( 2, nullptr );

    pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

	pD3D->SetRenderState( D3DRS_ZENABLE, TRUE );
	pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );	
	pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pD3D->SetRenderState( D3DRS_STENCILENABLE, FALSE );
		
	pD3D->SetPixelShader( mSkyboxPixelShader );
	pD3D->SetVertexShader( mSkyboxVertexShader );
	mShadersChangeCount += 2; 


	// Render Skybox
	if( camera->mSkybox ) {
		D3DXMATRIX wvp;
		D3DXMatrixTranslation( &wvp, camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z );

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushMatrix( wvp * camera->mViewProjection );
		pD3D->SetVertexShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

		pD3D->SetIndices( mSkyboxIndexBuffer );
		pD3D->SetStreamSource( 0, mSkyboxVertexBuffer, 0, sizeof( Vertex ));
		for( int i = 0; i < 5; i++ ) {
			pD3D->SetTexture( 0, camera->mSkybox->mTextures[i]->GetInterface() );
			++mTextureChangeCount;

			pD3D->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 20, i * 6, 2 );
			++mDIPCount;
			mRenderedTriangleCount += 2;
		}
	} 
	
	// Begin light occlusion queries
	pD3D->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );

	auto & pointLights = SceneFactory::GetPointLightList();
	for( auto & lWeak : pointLights ) {
		shared_ptr<PointLight> & pLight = lWeak.lock();
		if( pLight ) {
			if( pLight->mQueryDone ) {
				if( camera->mFrustum.IsSphereInside( pLight->GetPosition(), pLight->GetRange() ) && pLight->IsVisible() ) {
					bool found = false;
					for( auto & lit : camera->GetNearestPathPoint()->GetListOfVisibleLights() ) {
						if( lit.lock() == pLight ) {
							found = true;
						}
					}
					if( !found ) {
						pLight->pQuery->Issue( D3DISSUE_BEGIN );

						gpuFloatRegisterStack.Clear();
						gpuFloatRegisterStack.PushMatrix( SetUniformScaleTranslationMatrix( 1.25f * pLight->mRadius,  pLight->GetPosition() ) * camera->mViewProjection );
						pD3D->SetVertexShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

						mBoundingStar->DrawSubset( 0 );
						pD3D->SetVertexDeclaration( msVertexDeclaration );

						pLight->pQuery->Issue( D3DISSUE_END );
					}

					pLight->mInFrustum = true;						
					pLight->mQueryDone = false;
				} else {
					pLight->mInFrustum = false;
				}
			}
		}
	}

	for( auto & lWeak : pointLights ) {
		shared_ptr<PointLight> & pLight = lWeak.lock();
		if( pLight ) {
			bool inFrustum = camera->mFrustum.IsSphereInside( pLight->GetPosition(), pLight->GetRange() );
			if( pLight->IsVisible() && inFrustum ) {
				bool found = false;
				for( auto & lit : camera->GetNearestPathPoint()->GetListOfVisibleLights() ) {
					if( lit.lock() == pLight ) {
						found = true;
						break;
					}
				}
				DWORD pixelsVisible = 0;
				if( pLight->mInFrustum  && !pLight->mQueryDone ) {
					if( !found ) {
						HRESULT result = pLight->pQuery->GetData( &pixelsVisible, sizeof( pixelsVisible ), D3DGETDATA_FLUSH ) ;
						if( result == S_OK ) {
							pLight->mQueryDone = true;
							if( pixelsVisible > 0 ) {				
								// add light to light list of nearest path point of camera									
								camera->GetNearestPathPoint()->GetListOfVisibleLights().push_back( pLight );								
							}
						}
					}
				}
			}
		}
	}
		
	pD3D->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );

	// Bind G-Buffer Textures
	pD3D->SetTexture( 0, mDepthMap );
	pD3D->SetTexture( 1, mNormalMap );
	pD3D->SetTexture( 2, mDiffuseMap );
	mTextureChangeCount += 3;

	// Light passes rendered with additive blending
	pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );		
	pD3D->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// Apply ambient lighting
	pD3D->SetPixelShader( mAmbientPixelShader );
	++mShadersChangeCount;

	gpuFloatRegisterStack.Clear();
	gpuFloatRegisterStack.PushVector( mAmbientColor );
	pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

	RenderFullscreenQuad();
	
	// Render point lights
	pD3D->SetRenderState( D3DRS_STENCILENABLE, TRUE );
	
	float hdrLightIntensity = mHDREnabled ? 4.0f : 1.5f;

	pD3D->SetPixelShader( mDeferredLightShader );
	++mShadersChangeCount;

	for( auto lWeak : camera->GetNearestPathPoint()->GetListOfVisibleLights() ) {
		shared_ptr<PointLight> & pLight = lWeak.lock();
		if( pLight ) {
			if( pLight->IsVisible() ) {
				if( pLight->mInFrustum  ) {		
					bool useShadows = false;

					// So fucking heavy part, must be optimized !!!
					if( (camera->GetPosition() - pLight->GetPosition()).Length() < 20.0f ) {
						useShadows = mUsePointLightShadows;					

						// Render shadow cube map
						if( mUsePointLightShadows ) {
							pD3D->SetRenderState( D3DRS_STENCILENABLE, FALSE );
							pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
							pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

							CameraDirection directions[6] = {
								{ ruVector3( 1.0f,  0.0f,  0.0f), ruVector3(0.0f, 1.0f,  0.0f) },
								{ ruVector3(-1.0f,  0.0f,  0.0f), ruVector3(0.0f, 1.0f,  0.0f) },
								{ ruVector3( 0.0f,  1.0f,  0.0f), ruVector3(0.0f, 0.0f, -1.0f) },
								{ ruVector3( 0.0f, -1.0f,  0.0f), ruVector3(0.0f, 0.0f,  1.0f) },
								{ ruVector3( 0.0f,  0.0f,  1.0f), ruVector3(0.0f, 1.0f,  0.0f) },
								{ ruVector3( 0.0f,  0.0f, -1.0f), ruVector3(0.0f, 1.0f,  0.0f) }
							};

							pD3D->SetPixelShader( mShadowMapPixelShader );
							pD3D->SetVertexShader( mShadowMapVertexShader );
							mShadersChangeCount += 2;

							D3DXMATRIX projectionMatrix;
							D3DXMatrixPerspectiveFovLH( &projectionMatrix, 1.570796, 1.0f, 0.05f, 1024.0f );

							for( int face = 0; face < 6; ++face ) {
								IDirect3DSurface9 * faceSurface;
								mCubeShadowMap->GetCubeMapSurface( (D3DCUBEMAP_FACES)face, 0, &faceSurface );

								pD3D->SetRenderTarget( 0, faceSurface );
								pD3D->SetDepthStencilSurface( mDepthStencilSurface );
								pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
														
								ruVector3 eye = pLight->GetPosition();							
								ruVector3 at = eye + directions[face].target;
								ruVector3 up = directions[face].up;

								D3DXMATRIX viewMatrix;
								D3DXMatrixLookAtLH( &viewMatrix, &D3DXVECTOR3( eye.x, eye.y, eye.z ), &D3DXVECTOR3( at.x, at.y, at.z ), &D3DXVECTOR3( up.x, up.y, up.z )							);
							
								for( auto & texGroupPair : mDeferredMeshMap ) {
									pD3D->SetTexture( 7, texGroupPair.first );
									for( auto & weakMesh : texGroupPair.second ) {
										shared_ptr<Mesh> mesh = weakMesh.lock();
										auto & owners = mesh->GetOwners();
										for( auto & weakOwner : owners ) {
											shared_ptr<SceneNode> & pOwner = weakOwner.lock();								 

											if( mesh->mAABB.IsIntersectSphere( pOwner->GetPosition(), pLight->GetPosition(), pLight->GetRange())) {
												if( (pOwner->IsBone() ? false : pOwner->IsVisible()) || pOwner->IsSkinned() ) {
													// Load vertex shader constants
													gpuFloatRegisterStack.Clear();
													gpuFloatRegisterStack.PushMatrix( pOwner->GetWorldMatrix() * viewMatrix * projectionMatrix );
													gpuFloatRegisterStack.PushMatrix( pOwner->GetWorldMatrix() );
													for( auto bone : mesh->GetBones() ) {
														shared_ptr<SceneNode> boneNode = bone->mNode.lock();
														if( boneNode ) {
															gpuFloatRegisterStack.PushMatrix( TransformToMatrix( boneNode->GetRelativeTransform() * pOwner->GetLocalTransform() ));
														}
													}		
													pD3D->SetVertexShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

													gpuBoolRegisterStack.Clear();
													gpuBoolRegisterStack.Push( pOwner->IsSkinned() );
													gpuBoolRegisterStack.Push( TRUE );
													pD3D->SetVertexShaderConstantB( 0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount );

													// Load pixel shader constants
													gpuFloatRegisterStack.Clear();
													gpuFloatRegisterStack.PushVector( pLight->GetPosition() );
													pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

													gpuBoolRegisterStack.Clear();
													gpuBoolRegisterStack.Push( TRUE );
													pD3D->SetPixelShaderConstantB( 0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount );

													RenderMesh( mesh );			
												}
											}
										}
									}
								}

								faceSurface->Release();
							}
						
							pD3D->SetRenderTarget( 0, renderTarget );
							pD3D->SetDepthStencilSurface( mDefaultDepthStencil );

							pD3D->SetRenderState( D3DRS_STENCILENABLE, TRUE );
							pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
							pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

							pD3D->SetTexture( 6, mCubeShadowMap );
							++mTextureChangeCount;

							// Revert to light shader
							pD3D->SetPixelShader( mDeferredLightShader );
							pD3D->SetVertexShader( mQuadVertexShader );
							++mShadersChangeCount;
						}
					}

					pD3D->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
					pD3D->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
					pD3D->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );			

					pD3D->SetRenderState( D3DRS_ZENABLE, TRUE );
								
					if( pLight->GetPointTexture() ) {
						pD3D->SetTexture( 3, pLight->GetPointTexture()->mCubeTexture );
					} else {
						pD3D->SetTexture( 3, mWhiteCubeMap );
					}
					++mTextureChangeCount;

					// Load pixel shader constants
					gpuFloatRegisterStack.Clear();
					gpuFloatRegisterStack.PushMatrix( camera->invViewProjection );
					gpuFloatRegisterStack.PushIdentityMatrix();
					gpuFloatRegisterStack.PushVector( pLight->GetPosition() );
					gpuFloatRegisterStack.PushVector( pLight->GetColor() );
					gpuFloatRegisterStack.PushFloat( pLight->GetRange() );
					gpuFloatRegisterStack.PushFloat( hdrLightIntensity );
					pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );
										
					gpuBoolRegisterStack.Clear();
					gpuBoolRegisterStack.Push( TRUE );
					gpuBoolRegisterStack.Push( useShadows );
					pD3D->SetPixelShaderConstantB( 0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount );

					// Load vertex shader constants 
					gpuFloatRegisterStack.Clear();
					gpuFloatRegisterStack.PushMatrix( SetUniformScaleTranslationMatrix( 1.5f * pLight->mRadius, pLight->GetPosition() ) * camera->mViewProjection );
					pD3D->SetVertexShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

					// Render bounding sphere
					mBoundingSphere->DrawSubset( 0 );
					pD3D->SetVertexDeclaration( msVertexDeclaration );

					// Revert stencil
					pD3D->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
					pD3D->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
					pD3D->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );

					// Apply lighting to pixels, that marked by bounding sphere
					RenderFullscreenQuad();		
				}		
			}
		}
	}
	
	// Render spot lights
	pD3D->SetPixelShader( mDeferredLightShader );
	++mShadersChangeCount;

	auto & spotLights = SceneFactory::GetSpotLightList();
	for( auto & lWeak : spotLights ) {
		shared_ptr<SpotLight> & pLight = lWeak.lock();
		if( pLight ) {
			if( camera->mFrustum.IsSphereInside( pLight->GetPosition(), pLight->GetRange() ) && pLight->IsVisible()  ) {
				// If shadows enabled, render shadowmap first
				if( IsSpotLightShadowsEnabled() ) {
					pD3D->SetRenderState( D3DRS_STENCILENABLE, FALSE );
					pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
					pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
						
					pD3D->SetTexture( 4, nullptr );
					++mTextureChangeCount;

					pD3D->SetRenderTarget( 0, mShadowMapSurface );
					pD3D->SetDepthStencilSurface( mDepthStencilSurface );
					pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

					pD3D->SetPixelShader( mShadowMapPixelShader );
					pD3D->SetVertexShader( mShadowMapVertexShader );
					mShadersChangeCount += 2;

					pLight->BuildSpotProjectionMatrixAndFrustum();
						
					for( auto & texGroupPair : mDeferredMeshMap ) {
						pD3D->SetTexture( 7, texGroupPair.first );
						for( auto & weakMesh : texGroupPair.second ) {
							shared_ptr<Mesh> mesh = weakMesh.lock();
							auto & owners = mesh->GetOwners();
							for( auto & weakOwner : owners ) {
								shared_ptr<SceneNode> & pOwner = weakOwner.lock();	

								if( pOwner->IsBone() )  {
									continue;
								}

								if( pOwner->IsVisible() && pLight->GetFrustum().IsAABBInside( mesh->GetBoundingBox(), pOwner->GetPosition(), pOwner->GetWorldMatrix())) {
									// Load vertex shader constants
									gpuFloatRegisterStack.Clear();
									gpuFloatRegisterStack.PushMatrix( pOwner->GetWorldMatrix() * pLight->GetViewProjectionMatrix() );
									gpuFloatRegisterStack.PushMatrix( pOwner->GetWorldMatrix() );
									for( auto bone : mesh->GetBones() ) {
										shared_ptr<SceneNode> boneNode = bone->mNode.lock();
										if( boneNode ) {
											gpuFloatRegisterStack.PushMatrix( TransformToMatrix( boneNode->GetRelativeTransform() * pOwner->GetLocalTransform() ));
										}
									}		
									pD3D->SetVertexShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

									gpuBoolRegisterStack.Clear();
									gpuBoolRegisterStack.Push( pOwner->IsSkinned() );
									gpuBoolRegisterStack.Push( FALSE );
									pD3D->SetVertexShaderConstantB( 0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount );

									// Load pixel shader constants
									gpuFloatRegisterStack.Clear();
									gpuFloatRegisterStack.PushVector( pLight->GetPosition() );
									pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

									gpuBoolRegisterStack.Clear();
									gpuBoolRegisterStack.Push( FALSE );
									pD3D->SetPixelShaderConstantB( 0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount );								

									RenderMesh( mesh );
								}	
							}
						}
					}

					pD3D->SetRenderTarget( 0, renderTarget );
					pD3D->SetDepthStencilSurface( mDefaultDepthStencil );

					pD3D->SetRenderState( D3DRS_STENCILENABLE, TRUE );
					pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

					pD3D->SetTexture( 5, mShadowMap );
					++mTextureChangeCount;

					// Revert to light shader
					pD3D->SetPixelShader( mDeferredLightShader );
					pD3D->SetVertexShader( mQuadVertexShader );
					++mShadersChangeCount;
				} 

				pD3D->SetTexture( 4, pLight->GetSpotTexture()->GetInterface() );
				++mTextureChangeCount;

				pD3D->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
				pD3D->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
				pD3D->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

				// Setup bounding cone properties
				D3DXMATRIX scale;
				float height = pLight->GetRange() * 2.5;
				float radius = height * sinf( ( pLight->GetOuterAngle() * 0.75f ) * SIMD_PI / 180.0f );				
				D3DXMatrixScaling( &scale, radius, height, radius );

				// Load vertex shader constants
				gpuFloatRegisterStack.Clear();
				gpuFloatRegisterStack.PushMatrix( (scale * pLight->GetWorldMatrix()) * camera->mViewProjection );
				pD3D->SetVertexShaderConstantF( 0,gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

				// Render Oriented Bounding Cone
				mBoundingCone->DrawSubset( 0 );
				pD3D->SetVertexDeclaration( msVertexDeclaration );

				pLight->BuildSpotProjectionMatrixAndFrustum();

				// Load pixel shader constants
				gpuFloatRegisterStack.Clear();
				gpuFloatRegisterStack.PushMatrix( camera->invViewProjection );
				gpuFloatRegisterStack.PushMatrix( pLight->GetViewProjectionMatrix() );
				gpuFloatRegisterStack.PushVector( pLight->GetPosition() );
				gpuFloatRegisterStack.PushVector( pLight->GetColor() );
				gpuFloatRegisterStack.PushFloat( pLight->GetRange() );
				gpuFloatRegisterStack.PushFloat( hdrLightIntensity );
				pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

				gpuBoolRegisterStack.Clear();
				gpuBoolRegisterStack.Push( FALSE );
				gpuBoolRegisterStack.Push( mUseSpotLightShadows );
				pD3D->SetPixelShaderConstantB( 0, gpuBoolRegisterStack.GetPointer(), gpuBoolRegisterStack.mBooleanCount );

				pD3D->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
				pD3D->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
				pD3D->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );

				// Apply lighting to pixels, that marked by bounding cone
				RenderFullscreenQuad();
			}
		}
	}

	// Apply post-effects (HDR, FXAA, and so on)
	IDirect3DTexture9 * finalFrame = mFrame[0];

	pD3D->SetRenderState( D3DRS_STENCILENABLE, FALSE );	
	pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pD3D->SetRenderState( D3DRS_ZENABLE, FALSE );

	SetGenericSamplersFiltration( D3DTEXF_POINT, true );

	// Do HDR
	if( mHDREnabled ) {	
		// Build bloom texture
		pD3D->SetRenderTarget( 0, mBloomTextureSurface );
		pD3D->Clear( 0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

		pD3D->SetPixelShader( mBloomPixelShader );
		++mShadersChangeCount;

		pD3D->SetTexture( 0, mHDRFrame );
		++mTextureChangeCount;

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushVector( mBloomDX );
		gpuFloatRegisterStack.PushVector( mBloomDY );
		pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

		RenderFullscreenQuad();

		// Blur bloom vertically
		pD3D->SetRenderTarget( 0, mBloomBlurredSurface );
		pD3D->Clear( 0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

		pD3D->SetTexture( 0, mBloomTexture );
		++mTextureChangeCount;

		pD3D->SetPixelShader( mGaussianBlurShader );
		++mShadersChangeCount;

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushVector( mBloomDY );
		pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

		RenderFullscreenQuad();

		// Blur bloom horizontally
		pD3D->SetRenderTarget( 0, mBloomTextureSurface );
		pD3D->Clear( 0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

		pD3D->SetTexture( 0, mBloomBlurredTexture );
		++mTextureChangeCount;

		pD3D->SetPixelShader( mGaussianBlurShader );
		++mShadersChangeCount;

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushVector( mBloomDX );
		pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

		RenderFullscreenQuad();

		// Calculate HDR-frame luminance
		pD3D->SetTexture( 0, mHDRFrame );
		++mTextureChangeCount;

		pD3D->SetRenderTarget( 0, mScaledSceneSurf );
		pD3D->Clear( 0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

		pD3D->SetPixelShader( mScaleScenePixelShader );
		++mShadersChangeCount;

		RenderFullscreenQuad();

		pD3D->SetPixelShader( mDownScalePixelShader );
		++mShadersChangeCount;

		pD3D->SetTexture( 0, mScaledScene );
		++mTextureChangeCount;

		for( int i = 0; i < mHDRDownSampleCount; i++ ) {
			float pixelSize = 1.0f / static_cast<float>( IntegerPow( 2, mHDRDownSampleCount - i ));	

			pD3D->SetRenderTarget( 0, mDownSampSurf[ i ] );
			pD3D->Clear( 0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

			gpuFloatRegisterStack.Clear();
			gpuFloatRegisterStack.PushFloat( pixelSize );
			pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

			RenderFullscreenQuad();

			pD3D->SetTexture( 0, mDownSampTex[ i ] );
			++mTextureChangeCount;
		}

		// Do adaptation (!actual adaptation speed depends on the FPS, so this must be fixed!)
		IDirect3DTexture9 * pTexSwap = mAdaptedLuminanceLast;
		mAdaptedLuminanceLast = mAdaptedLuminanceCurrent;
		mAdaptedLuminanceCurrent = pTexSwap;

		IDirect3DSurface9 * pSurfAdaptedLum = NULL;
		mAdaptedLuminanceCurrent->GetSurfaceLevel( 0, &pSurfAdaptedLum );

		pD3D->SetRenderTarget( 0, pSurfAdaptedLum );
		pD3D->SetTexture( 0, mDownSampTex[ mHDRDownSampleCount - 1 ] );
		++mTextureChangeCount;
		pD3D->SetTexture( 1, mAdaptedLuminanceLast );
		++mTextureChangeCount;
		pD3D->SetTexture( 2, mBloomTexture );
		++mTextureChangeCount;
		
		pD3D->SetPixelShader( mAdaptationPixelShader );
		++mShadersChangeCount;

		// Set pixel shader constants
		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushFloat( 0.55f );
		pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

		RenderFullscreenQuad();

		pSurfAdaptedLum->Release();
		
		// And finally do tone-mapping
		pD3D->SetRenderTarget( 0, mFrameSurface[0] );
		pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

		pD3D->SetTexture( 0, mHDRFrame );
		++mTextureChangeCount;
		pD3D->SetTexture( 1, mAdaptedLuminanceCurrent );
		++mTextureChangeCount;

		pD3D->SetPixelShader( mToneMapShader );
		++mShadersChangeCount;

		RenderFullscreenQuad();

		finalFrame = mFrame[0];
	};

	// Do blending for transparent lines (remember interlace rendering for transparent objects)
	pD3D->SetRenderTarget( 0, mFrameSurface[1] );
	pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

	pD3D->SetTexture( 0, finalFrame );
	++mTextureChangeCount;

	pD3D->SetTexture( 1, mDiffuseMap );
	++mTextureChangeCount;

	pD3D->SetPixelShader( mDeferredBlending );
	++mShadersChangeCount;

	gpuFloatRegisterStack.Clear();
	gpuFloatRegisterStack.PushFloat( 1.0f / GetResolutionHeight() );
	pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

	RenderFullscreenQuad();

	finalFrame = mFrame[1];
	
	// Do FXAA
	if( mFXAAEnabled ) {
		pD3D->SetRenderTarget( 0, mFrameSurface[0] );
		pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

		pD3D->SetTexture( 0, finalFrame );
		++mTextureChangeCount;

		pD3D->SetPixelShader( mFXAAPixelShader );
		++mShadersChangeCount;

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushFloat( GetResolutionWidth(), GetResolutionHeight() );
		pD3D->SetPixelShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

		RenderFullscreenQuad();

		finalFrame = mFrame[0];
	}

	// Postprocessing
	if( mHDREnabled ) {
		pD3D->SetRenderState( D3DRS_SRGBWRITEENABLE, TRUE );
	}

	pD3D->SetRenderTarget( 0, mpBackBuffer );
	pD3D->Clear( 0, 0, D3DCLEAR_TARGET , D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

	pD3D->SetTexture( 0, finalFrame );
	++mTextureChangeCount;

	pD3D->SetPixelShader( mSkyboxPixelShader );
	++mShadersChangeCount;

	RenderFullscreenQuad();
			
	if( IsAnisotropicFilteringEnabled() ) {
		SetGenericSamplersFiltration( D3DTEXF_ANISOTROPIC, false );
	} else {
		SetGenericSamplersFiltration( D3DTEXF_LINEAR, false );
	}

	pD3D->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
	
	
	//********************************
    // Forward rendering for transparent meshes

	pD3D->SetRenderState( D3DRS_ZENABLE, TRUE );

	pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pD3D->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	
	pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pD3D->SetRenderState( D3DRS_STENCILENABLE, FALSE );

	pD3D->SetPixelShader( mParticleSystemPixelShader );
	++mShadersChangeCount;

	ruVector3 rightVect = ruVector3( camera->mView._11, camera->mView._21, camera->mView._31 ).Normalize();
	ruVector3 upVect = ruVector3( camera->mView._12, camera->mView._22, camera->mView._32 ).Normalize();
	ruVector3 frontVect = ruVector3( camera->mView._13, camera->mView._23, camera->mView._33 ).Normalize();

	auto & particleSystems = SceneFactory::GetParticleSystemList();
	for( auto pWeak : particleSystems ) {
		shared_ptr<ParticleSystem> & particleEmitter = pWeak.lock();

		if( !particleEmitter->IsVisible() || !particleEmitter->IsEnabled() || !particleEmitter->HasAliveParticles() ) {
			continue;
		}

		particleEmitter->Update();
			
		if( particleEmitter->mTexture ) {
			pD3D->SetTexture( 0, particleEmitter->mTexture->GetInterface() );
			++mTextureChangeCount;
		}

		pD3D->SetStreamSource( 0, particleEmitter->mVertexBuffer, 0, sizeof( Vertex ));
		pD3D->SetIndices( particleEmitter->mIndexBuffer );
			
		camera->EnterDepthHack( particleEmitter->GetDepthHack() );
				
		if( particleEmitter->mUseLighting ) {
			pD3D->SetVertexShader( mParticleSystemVertexShader );
			++mShadersChangeCount;
		} else {
			pD3D->SetVertexShader( mParticleSystemVertexShader );
			++mShadersChangeCount;
		}

		gpuFloatRegisterStack.Clear();
		gpuFloatRegisterStack.PushMatrix( camera->mViewProjection );
		gpuFloatRegisterStack.PushMatrix( particleEmitter->GetWorldMatrix() );
		gpuFloatRegisterStack.PushFloat( upVect.x, upVect.y, upVect.z, 0.0f );
		gpuFloatRegisterStack.PushFloat( rightVect.x, rightVect.y, rightVect.z, 0.0f );
		gpuFloatRegisterStack.PushFloat( frontVect.x, frontVect.y, frontVect.z, 0.0f );
		
		pD3D->SetVertexShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

		++mDIPCount;
		pD3D->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, particleEmitter->mAliveParticleCount * 4, 0, particleEmitter->mAliveParticleCount * 2 );
		mRenderedTriangleCount += particleEmitter->mAliveParticleCount * 2;

		camera->LeaveDepthHack();				
	}

    // Render gui on top of all    
	pD3D->SetRenderState( D3DRS_ZENABLE, FALSE );

	pD3D->SetPixelShader( mGUIPixelShader );
	pD3D->SetVertexShader( mGUIVertexShader );
	mShadersChangeCount += 2;

	// Load vertex shader matrices
	gpuFloatRegisterStack.Clear();
	gpuFloatRegisterStack.PushMatrix( mOrthoProjectionMatrix );
	pD3D->SetVertexShaderConstantF( 0, gpuFloatRegisterStack.GetPointer(), gpuFloatRegisterStack.mRegisterCount );

	pD3D->SetStreamSource( 0, mRectVertexBuffer, 0, sizeof( Vertex ));

	// Render all rectangles
	auto & rects = GUIFactory::GetRectList();
	for( auto & rWeak : rects ) {
		shared_ptr<GUIRect> & pRect = rWeak.lock();
		if( pRect ) {
			if( pRect->IsVisible() ) {
				RenderRect( pRect );
			}
		}
	}

	auto & texts = GUIFactory::GetTextList();
	for( auto tWeak : texts ) {
		shared_ptr<GUIText> & pText = tWeak.lock();
		if( pText->IsVisible() ) {
			Vertex * vertices = nullptr;
			Triangle * triangles = nullptr;
			int n = 0;
			int totalLetters = 0;

			shared_ptr<BitmapFont> & font = pText->GetFont();
			
			mTextVertexBuffer->Lock( 0, mTextMaxChars * 4 * sizeof( Vertex ), reinterpret_cast<void**>( &vertices ), D3DLOCK_DISCARD );				
			mTextIndexBuffer->Lock( 0, mTextMaxChars * 2 * sizeof( Triangle ), reinterpret_cast<void**>( &triangles ), D3DLOCK_DISCARD );				

			ruVector2 position = pText->GetGlobalPosition();
			ruVector2 caret = position;
			for( auto & line : pText->mLines ) {
				caret.x = line.mX + position.x;
				caret.y = line.mY + position.y;
				for( unsigned char symbol : line.mSubstring ) {
					CharMetrics & charMetr = font->mCharsMetrics[ symbol ];

					int x = caret.x + charMetr.bitmapLeft;
					int y = caret.y - charMetr.bitmapTop + font->mGlyphSize;

					// vertices
					*(vertices++) = Vertex( ruVector3( x, y, 0.0f ), charMetr.texCoords[0], ruVector4( pText->GetColor(), pText->GetAlpha() / 255.0f ));
					*(vertices++) = Vertex( ruVector3( x + font->mGlyphSize, y, 0.0f ), charMetr.texCoords[1], ruVector4( pText->GetColor(), pText->GetAlpha() / 255.0f ));
					*(vertices++) = Vertex( ruVector3( x + font->mGlyphSize, y + font->mGlyphSize, 0.0f ), charMetr.texCoords[2], ruVector4( pText->GetColor(), pText->GetAlpha() / 255.0f ));
					*(vertices++) = Vertex( ruVector3( x, y + font->mGlyphSize, 0.0f ), charMetr.texCoords[3], ruVector4( pText->GetColor(), pText->GetAlpha() / 255.0f ));

					// indices
					*(triangles++) = Triangle( n, n + 1, n + 2 );
					*(triangles++) = Triangle( n, n + 2, n + 3 );

					n += 4;
					totalLetters++;

					if( symbol != '\n' ) {
						caret.x += charMetr.advanceX;
					}
				}
			}

			mTextVertexBuffer->Unlock();
			mTextIndexBuffer->Unlock();
				
			if( totalLetters > 0 ) {
				pD3D->SetTexture( 0, font->mAtlas );
				++mTextureChangeCount;

				pD3D->SetStreamSource( 0, mTextVertexBuffer, 0, sizeof( Vertex ));
				pD3D->SetIndices( mTextIndexBuffer );

				pD3D->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, totalLetters * 4, 0, totalLetters * 2 );
				mRenderedTriangleCount += totalLetters * 2;
				++mDIPCount;
			}
		}		
	}

	// Render Cursor on top of all
	if( GetCursor() ) {
		pD3D->SetStreamSource( 0, mRectVertexBuffer, 0, sizeof( Vertex ));
		if( GetCursor()->IsVisible() ) {
			GetCursor()->SetPosition( ruInput::GetMouseX() / GetGUIWidthScaleFactor(), ruInput::GetMouseY() / GetGUIHeightScaleFactor());
			RenderRect( GetCursor() );
		}
	}

    // End rendering
    pD3D->EndScene();
    pD3D->Present( 0, 0, 0, 0 );
}

/*
=========
UpdateWorld
	- This method should be called in game update cycle (independent from rendering) with a fixed period
	- Performs global transforms calculations of all scene nodes
	- Performs keyframe and skeletal animation
=========
*/
void Renderer::UpdateWorld() {
	// build view and projection matrices, frustum, also attach sound listener to camera
	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		camera->Update();
	}

	auto & nodes = SceneFactory::GetNodeList();

	for( auto & pWeak : nodes ) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if( node ) {
			node->CalculateGlobalTransform();
			// update all sounds attached to node, and physical interaction sounds( roll, hit )
			node->UpdateSounds();
			if( !node->IsSkinned() ) {
				node->PerformAnimation();
			}
			if( camera ) {
				node->CheckFrustum( camera.get() );
			}
		}
	}

	// skinned animation is based on transforms of other nodes, so skin meshes in the end of all
	for( auto & pWeak : nodes ) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if( node ) {
			if( node->IsSkinned() ) {
				node->PerformAnimation();
			}
		} 
	}

	// Update GUI nodes
	auto & guiNodes = GUIFactory::GetNodeList();
	for( auto & nWeak : guiNodes ) {
		shared_ptr<GUINode> & pNode = nWeak.lock();
		if( pNode ) {
			pNode->DoActions();
		}
	}

	// Update sound subsystem
	pfSystemUpdate();
}


void Renderer::OnLostDevice() {
	mDepthSurface.Reset();
	mNormalSurface.Reset();
	mDiffuseSurface.Reset();
	mDepthMap.Reset();
	mNormalMap.Reset();
	mDiffuseMap.Reset();

	mBoundingStar.Reset();
	mBoundingSphere.Reset();
	mBoundingCone.Reset();	
	mHDRFrameSurface.Reset();
	mHDRFrame.Reset();
	for( int i = 0; i < 2; i++ ) {
		mFrameSurface[i].Reset();
		mFrame[i].Reset();
	}

	for( RendererComponent * pComponent : RendererComponent::msComponentList ) {
		pComponent->OnLostDevice();
	}	

	Reset();	
}

void Renderer::Reset() {
	mpDevice->Reset( &mPresentParameters );
	SetDefaults();
	OnResetDevice();
}

void Renderer::ChangeVideomode( int width, int height, bool fullscreen, bool vsync ) {
	if ( width == 0 ) {
		width = mNativeResolutionWidth;
	}
	if ( height == 0 ) {
		height = mNativeResolutionHeight;
	}

	mResWidth = width;
	mResHeight = height;

	D3DDISPLAYMODE displayMode = { 0 };
	mpDirect3D->GetAdapterDisplayMode ( D3DADAPTER_DEFAULT, &displayMode );

	// present parameters
	memset( &mPresentParameters, 0, sizeof( mPresentParameters ));

	mPresentParameters.BackBufferCount = 2;
	mPresentParameters.EnableAutoDepthStencil = TRUE;
	if( vsync ) {
		mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	} else {
		mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	mPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	mPresentParameters.BackBufferWidth = width;
	mPresentParameters.BackBufferHeight = height;
	mPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	mPresentParameters.hDeviceWindow = mWindowHandle;
	if ( fullscreen ) {
		mPresentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;		
		mPresentParameters.Windowed = FALSE;
		mPresentParameters.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
	} else {
		mPresentParameters.BackBufferFormat = displayMode.Format;
		mPresentParameters.Windowed = TRUE;
	}

	// no multisampling, because of deferred shading
	mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	mPresentParameters.MultiSampleQuality = 0;

	if( fullscreen ) {
		SetWindowLongPtr( mWindowHandle, GWL_STYLE, WS_POPUP );
		SetWindowPos( mWindowHandle, HWND_TOP, 0, 0, width, height, SWP_SHOWWINDOW );
	} else {
		RECT rect = { 0, 0, width, height };
		AdjustWindowRect( &rect, WS_OVERLAPPEDWINDOW, false );
		SetWindowLongPtr( mWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW );
		SetWindowPos( mWindowHandle, HWND_TOP, 0, 0, rect.right, rect.bottom, SWP_SHOWWINDOW );
	}

	OnLostDevice();
}

void Renderer::OnResetDevice() {
	pD3D->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &mpBackBuffer );

	RendererComponent::ResetPriority highPriority = RendererComponent::ResetPriority::High;
	for( RendererComponent * pComponent : RendererComponent::msComponentList ) {
		if( pComponent->mResetPriority == highPriority ) {
			pComponent->OnResetDevice();
		}
	}

	RendererComponent::ResetPriority mediumPriority = RendererComponent::ResetPriority::Medium;
	for( RendererComponent * pComponent : RendererComponent::msComponentList ) {
		if( pComponent->mResetPriority == mediumPriority ) {
			pComponent->OnResetDevice();
		}
	}

	RendererComponent::ResetPriority lowPriority = RendererComponent::ResetPriority::Low;
	for( RendererComponent * pComponent : RendererComponent::msComponentList ) {
		if( pComponent->mResetPriority == lowPriority ) {
			pComponent->OnResetDevice();
		}
	}	
}

void Renderer::SetSpotLightShadowMapSize( int size ) {
	
}

int Renderer::GetDIPCount() {
	return mDIPCount;
}

float Renderer::GetResolutionHeight() {
	return mResHeight;
}

float Renderer::GetResolutionWidth() {
	return mResWidth;
}

bool Renderer::IsSpotLightShadowsEnabled() {
	return mUseSpotLightShadows;
}

bool Renderer::IsPointLightShadowsEnabled() {
	return mUsePointLightShadows;
}

void Renderer::SetSpotLightShadowsEnabled( bool state ) {
	mUseSpotLightShadows = state;
}

void Renderer::SetPointLightShadowsEnabled( bool state ) {
	mUsePointLightShadows = state;
}

void Renderer::SetAmbientColor( ruVector3 ambColor ) {
	mAmbientColor = ambColor;
}

ruVector3 Renderer::GetAmbientColor() {
	return mAmbientColor;
}

void Renderer::SetHDREnabled( bool state ) {
	mHDREnabled = state;
}

bool Renderer::IsHDREnabled() {
	return mHDREnabled;
}

void Renderer::SetFXAAEnabled( bool state ) {
	mFXAAEnabled = state;
}

float Renderer::GetGUIHeightScaleFactor() const
{
	return mResHeight / ruVirtualScreenHeight;
}

bool Renderer::IsFXAAEnabled() {
	return mFXAAEnabled;
}

void Renderer::Shutdown() {
	mRunning = false;
}

shared_ptr<Cursor> & Renderer::GetCursor()
{
	return mCursor;
}

int Renderer::GetTextureChangeCount() {
	return mTextureChangeCount;
}

int Renderer::GetShaderChangeCount()
{
	return mShadersChangeCount;
}

void Renderer::SetTextureStoragePath( const string & path ) {
	mTextureStoragePath = path;
}

std::string Renderer::GetTextureStoragePath() {
	return mTextureStoragePath;
}

void Renderer::SetGenericSamplersFiltration( D3DTEXTUREFILTERTYPE filter, bool disableMips ) {
	// number of generic samplers (i.e. for diffuse, normal and height textures )
	const int genericSamplersCount = 5;

	if( filter == D3DTEXF_NONE ) { // invalid argument to min and mag filters
		for( int i = 0; i < genericSamplersCount; i++ ) {
			pD3D->SetSamplerState ( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			pD3D->SetSamplerState ( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
	} else if( filter == D3DTEXF_LINEAR ) {
		for( int i = 0; i < genericSamplersCount; i++ ) {
			pD3D->SetSamplerState ( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			pD3D->SetSamplerState ( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
	} else if( filter == D3DTEXF_ANISOTROPIC ) {
		pD3D->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
		pD3D->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		// it's too expensive to set anisotropic filtration to normal and height maps, so set linear
		for( int i = 1; i < genericSamplersCount; i++ ) {
			pD3D->SetSamplerState ( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			pD3D->SetSamplerState ( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
	}

	// mip filters
	if( filter == D3DTEXF_NONE || disableMips ) {
		for( int i = 0; i < genericSamplersCount; i++ ) {
			pD3D->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		}
	} else if( filter == D3DTEXF_POINT ) {
		for( int i = 0; i < genericSamplersCount; i++ ) {
			pD3D->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
		}
	} else if( filter == D3DTEXF_LINEAR || filter == D3DTEXF_ANISOTROPIC ) {
		for( int i = 0; i < genericSamplersCount; i++ ) {
			pD3D->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		}
	}
}

void Renderer::SetAnisotropicTextureFiltration( bool state ) {
	mAnisotropicFiltering = state;
}

bool Renderer::IsAnisotropicFilteringEnabled() {
	return mAnisotropicFiltering;
}

void Renderer::SetDefaults() {
	pD3D->SetRenderState ( D3DRS_LIGHTING, FALSE );
	pD3D->SetRenderState( D3DRS_ZENABLE, TRUE );
	pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
	pD3D->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );

	pD3D->SetRenderState( D3DRS_STENCILREF, 0x0 );
	pD3D->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
	pD3D->SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
	pD3D->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE );
	pD3D->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
	pD3D->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR );
	pD3D->SetRenderState( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_INCR );

	// setup samplers
	pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	pD3D->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	pD3D->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );    
	pD3D->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	pD3D->SetSamplerState ( 3, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	pD3D->SetSamplerState ( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ); 	
	pD3D->SetSamplerState ( 3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	pD3D->SetSamplerState( 4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pD3D->SetSamplerState( 4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	D3DCAPS9 dCaps;
	mpDirect3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &dCaps );
	if( dCaps.MaxAnisotropy > 4 ) {
		dCaps.MaxAnisotropy = 4;
	}

	pD3D->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy );
	pD3D->SetSamplerState ( 1, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy );
	pD3D->SetSamplerState ( 2, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy );
	pD3D->SetSamplerState ( 3, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy );

	SetAnisotropicTextureFiltration( true );

	SetParallaxEnabled( true );
}

void Renderer::RenderMesh( const shared_ptr<Mesh> & mesh ) {
	pD3D->SetStreamSource( 0, mesh->mVertexBuffer, 0, sizeof( Vertex ));
	pD3D->SetIndices( mesh->mIndexBuffer );

	mpDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, mesh->mVertices.size(), 0, mesh->mTriangles.size() );
	mRenderedTriangleCount += mesh->mTriangles.size();
	++mDIPCount;
}


DWORD * Renderer::ReadEntireFile( const char * fileName )
{
	ifstream pFile( fileName, ios_base::binary | ios_base::in );
	if( !pFile.good() ) {
		Log::Error( StringBuilder( "Failed to load pixel shader from '" ) << fileName << "' !" );
	}
	pFile.seekg( 0, ios_base::end );
	UINT fSize = pFile.tellg();
	pFile.seekg( 0, ios_base::beg );
	DWORD * binaryData = new DWORD[ fSize ];
	pFile.read( reinterpret_cast<char*>( binaryData ), fSize );
	return binaryData;
}

void Renderer::LoadPixelShader( COMPtr<IDirect3DPixelShader9> & pixelShader, const char * fileName )
{
	DWORD * binaryData = ReadEntireFile( fileName );
	if( FAILED( pD3D->CreatePixelShader( binaryData, &pixelShader ))) {
		Log::Error( StringBuilder( "Unable to create pixel shader from '" ) << fileName << "' !" );
	}
	delete binaryData;
}

void Renderer::LoadVertexShader( COMPtr<IDirect3DVertexShader9> & vertexShader, const char * fileName )
{
	DWORD * binaryData = ReadEntireFile( fileName );
	if( FAILED( pD3D->CreateVertexShader( binaryData, &vertexShader ))) {
		Log::Error( StringBuilder( "Unable to create vertex shader from '" ) << fileName << "' !" );
	}
	delete binaryData;
}

void Renderer::RenderFullscreenQuad()
{
	pD3D->SetVertexShader( mQuadVertexShader );
	++mShadersChangeCount;

	pD3D->SetVertexShaderConstantF( 0, &mOrthoProjectionMatrix.m[0][0], 4 );		
	pD3D->SetStreamSource( 0, mQuadVertexBuffer, 0, sizeof( Vertex ));

	pD3D->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
	++mDIPCount;
	mRenderedTriangleCount += 2;
}

void Renderer::RenderRect( const shared_ptr<GUIRect> & r )
{
	if( !r->GetTexture() )  {
		return;
	}
	void * data = nullptr;
	Vertex vertices[6];
	r->CalculateTransform();
	r->GetSixVertices( vertices );
	mRectVertexBuffer->Lock( 0, 0, &data, D3DLOCK_DISCARD );
	memcpy( data, vertices, 6 * sizeof( Vertex ));
	mRectVertexBuffer->Unlock( );
	pD3D->SetTexture( 0, std::dynamic_pointer_cast<Texture>(r->GetTexture())->GetInterface() );
	pD3D->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
	mRenderedTriangleCount += 2;
	++mDIPCount;
}

float Renderer::GetGUIWidthScaleFactor() const {
	return mResWidth / ruVirtualScreenWidth;
}

void Renderer::SetParallaxEnabled( bool state ) {
	mParallaxEnabled = state;
}

void Renderer::AddMesh( const shared_ptr<Mesh> & mesh ) {
	mesh->CalculateAABB();
	// pass it to deferred renderer
	if( mesh->mDiffuseTexture ) {
		auto textureGroup = mDeferredMeshMap.find( mesh->mDiffuseTexture->GetInterface() );

		if( textureGroup == mDeferredMeshMap.end()) {
			mDeferredMeshMap[ mesh->mDiffuseTexture->GetInterface() ] = vector<weak_ptr<Mesh>>();
		}

		mDeferredMeshMap[ mesh->mDiffuseTexture->GetInterface() ].push_back( mesh );
	}
}

bool Renderer::IsParallaxEnabled() {
	return mParallaxEnabled;
}

void Renderer::SetCursorVisible( bool state ) {
	if( state ) {
		if( mCursor ) {
			mCursor->Show();
		} else {
			::ShowCursor( 1 );
			pD3D->ShowCursor( 1 );
		}
	} else {
		if( mCursor ) {
			mCursor->Hide();
		} else {
			::ShowCursor( 0 );
			pD3D->ShowCursor( 0 );
		}
	}
}

void Renderer::SetCursor( shared_ptr<ruTexture> texture, int w, int h ) {
	mCursor = shared_ptr<Cursor>( new Cursor( w, h, std::dynamic_pointer_cast<Texture>( texture )));
}
