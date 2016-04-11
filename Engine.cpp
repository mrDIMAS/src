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
#include "ParticleSystemRenderer.h"
#include "GUIRenderer.h"
#include "Light.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Engine.h"
#include "Physics.h"
#include "BitmapFont.h"
#include "TextRenderer.h"
#include "SceneFactory.h"
#include "Cursor.h"
#include "Shader.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Mesh.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Camera.h"
#include "Skybox.h"
#include "Utility.h"
#include "Engine.h"
#include "SceneFactory.h"
#include <random>

IDirect3DDevice9 * pD3D;
unique_ptr<Engine> pEngine;

struct A8R8G8B8Pixel {
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct XYZNormalVertex {
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
};

#define StaticArraySize( array ) (sizeof( array ) / sizeof( array[0] ))

D3DXMATRIX SetUniformScaleTranslationMatrix( float s, const ruVector3 & p ) {
	return D3DXMATRIX ( s, 0.0f, 0.0f, 0.0f,
		0.0f, s, 0.0f, 0.0f,
		0.0f, 0.0f, s, 0.0f,
		p.x, p.y, p.z, 1.0f );
}

Engine::Engine() : 
	mpParticleSystemRenderer( nullptr ), 
	mpTextRenderer( nullptr ),
	mpGUIRenderer( nullptr ), 
	mAmbientColor( 0.05, 0.05, 0.05 ), 
	mUsePointLightShadows( false ),
	mUseSpotLightShadows( false ), 
	mHDREnabled( false ), 
	mRunning( true ), 
	mFXAAEnabled( false ),
	mTextureStoragePath( "data/textures/generic/" ), 
	mParallaxEnabled( true ) 
{

}

Engine::~Engine() {	
    while( Timer::msTimerList.size() ) {
        delete Timer::msTimerList.front();
    }
    for( auto & kv : CubeTexture::all ) {
        delete kv.second;
    }
    Physics::DestructWorld();
}

LRESULT CALLBACK WindowProcess( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch ( msg ) {
	case WM_DESTROY:
		ruEngine::Free();
		PostQuitMessage ( 0 );
		break;
	case WM_ERASEBKGND:
		return 0;
	}	
	return DefWindowProc ( wnd, msg, wParam, lParam );
}

void Engine::Initialize( int width, int height, int fullscreen, char vSync ) {
    if ( width == 0 || height == 0 ) {
        width = GetSystemMetrics ( SM_CXSCREEN );
		height = GetSystemMetrics ( SM_CYSCREEN );
		Log::Write( "Zero passed as a resolution, using desktop native resolution." );
    }
  
    if( !CreateRenderWindow( width, height, fullscreen )) {
		Log::Error( "Failed to create render window! Engine initialization failed!" );
    }

    // try to create Direct3D9
    mpDirect3D.Set( Direct3DCreate9( D3D_SDK_VERSION ));

    // epic fail
    if( !mpDirect3D ) {
		Log::Error( "Failed to Direct3DCreate9! Ensure, that you have latest video drivers! Engine initialization failed!" );
    } else {
		Log::Write( "Direct3D 9 Created successfully!" );
	}

    // check d3d caps, to ensure, that user have modern hardware
    D3DCAPS9 dCaps;
    mpDirect3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &dCaps );

	// epic fail
    if( D3DSHADER_VERSION_MAJOR( dCaps.PixelShaderVersion ) < 2 ) {
		mpDirect3D->Release();
        Log::Error( "Your graphics card doesn't support Pixel Shader 2.0. Engine initialization failed! Buy a modern video card!" );        
    }

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

    // no multisampling, because of deferred shading
    mPresentParameters.MultiSampleQuality = 0;
	mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;

    // create device
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

    CreatePhysics( );

	pfSystemCreateLogFile( "ProjectF.log" ); 

	pfSystemEnableMessagesOutputToConsole(); 
	pfSystemEnableMessagesOutputToLogFile(); 

    pfSystemInit( );
    pfSetListenerDopplerFactor( 0 );

    mpParticleSystemRenderer = make_shared<ParticleSystemRenderer>();
    mpTextRenderer = make_shared<TextRenderer>();
    mpGUIRenderer = make_shared<GUIRenderer>();
	mAmbientColor = ruVector3( 0.05, 0.05, 0.05 );
	mUsePointLightShadows = false;
	mUseSpotLightShadows = false;
	mHDREnabled = false;
	mRunning = true;
	mFXAAEnabled = false;
	mPaused = false;
	mChangeVideomode = false;
	mTextureStoragePath = "data/textures/generic/";

	//***************************************************************
	// Init Deferred Rendering Stuff

	// Create render targets 
	D3DXCreateTexture( pD3D, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT, &mHDRFrame );
	mHDRFrame->GetSurfaceLevel( 0, &mHDRFrameSurface );

	for( int i = 0; i < 2; i++ ) {
		D3DXCreateTexture( pD3D, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &mFrame[i] );
		mFrame[i]->GetSurfaceLevel( 0, &mFrameSurface[i] );
	}

	// FXAA Pixel Shader
	mFXAAPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/fxaa.pso" ));

	// Create G-Buffer ( Depth: R32F, Diffuse: ARGB8, Normal: ARGB8 )
	pD3D->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mDepthMap, nullptr );
	pD3D->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mNormalMap, nullptr );
	pD3D->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mDiffuseMap, nullptr );

	mDepthMap->GetSurfaceLevel( 0, &mDepthSurface );
	mNormalMap->GetSurfaceLevel( 0, &mNormalSurface );
	mDiffuseMap->GetSurfaceLevel( 0, &mDiffuseSurface );

	// Ambient light pixel shader
	mAmbientPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredAmbientLight.pso" ));

	// Parallax occlusion mapping shaders
	mGBufferVertexShaderPOM = unique_ptr<VertexShader>( new VertexShader( "data/shaders/deferredGBufferPOM.vso" ));
	mGBufferPixelShaderPOM = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredGBufferPOM.pso" ));
	// Standard GBuffer shader
	mGBufferVertexShader = unique_ptr<VertexShader>( new VertexShader( "data/shaders/deferredGBuffer.vso" ));
	mGBufferPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredGBuffer.pso" ));
	// Standard GBuffer shader with skinning
	mGBufferVertexShaderSkin = unique_ptr<VertexShader>( new VertexShader( "data/shaders/deferredGBufferSkin.vso" ));
	mGBufferPixelShaderSkin = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredGBufferSkin.pso" ));

	// Init HDR Stuff
	int scaledSize = IntegerPow( 2, mHDRDownSampleCount + 1 );
	D3DXCreateTexture( pD3D, scaledSize, scaledSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mScaledScene );
	mScaledScene->GetSurfaceLevel( 0, &mScaledSceneSurf );

	for( int i = 0; i < mHDRDownSampleCount; i++ ) {
		int size = IntegerPow( 2, mHDRDownSampleCount - i );
		D3DXCreateTexture( pD3D, size, size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mDownSampTex[ i ]);
	}

	D3DXCreateTexture( pD3D, 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mAdaptedLuminanceLast );
	D3DXCreateTexture( pD3D, 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mAdaptedLuminanceCurrent );

	for( int i = 0; i < mHDRDownSampleCount; i++ ) {
		mDownSampTex[i]->GetSurfaceLevel( 0, &mDownSampSurf[i] );
	}

	mToneMapShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/hdrTonemap.pso" ));
	mScaleScenePixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/hdrScale.pso" ));
	mAdaptationPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/hdrAdaptation.pso" ));
	mDownScalePixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/hdrDownscale.pso" ));

	// Skybox shader
	mSkyboxVertexShader = unique_ptr<VertexShader>( new VertexShader( "data/shaders/skybox.vso" ));
	mSkyboxPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/skybox.pso" ));

	// Point light pixel shaders
	mPointLightPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredPointLight.pso" ));
	mPointLightPixelShaderTexProj = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredPointLightTexProj.pso" ));

	// Spot light pixel shaders
	mSpotLightPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredSpotLight.pso" ));
	mSpotLightPixelShaderShadows = unique_ptr<PixelShader>( new PixelShader( "data/shaders/deferredSpotLightShadows.pso" ));

	// Spot light shadow map
	const int shadowMapSize = 512;

	mSpotLightShadowMapVertexShader = unique_ptr<VertexShader>( new VertexShader( "data/shaders/spotShadowMap.vso" ));
	mSpotLightShadowMapPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/spotShadowMap.pso" ));

	pD3D->CreateTexture( shadowMapSize, shadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mShadowMap, nullptr );
	mShadowMap->GetSurfaceLevel( 0, &mShadowMapSurface );

	pD3D->GetDepthStencilSurface( &mDefaultDepthStencil );
	pD3D->CreateDepthStencilSurface( shadowMapSize, shadowMapSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &mDepthStencilSurface, 0 );

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
	D3DXMatrixMultiply( &transform, &rot90, &tran );
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

	//***************************************************************
	// Init Forward Rendering Stuff
	mTransparentVertexShader = make_shared<VertexShader>( "data/shaders/forwardTransparent.vso" );
	mTransparentPixelShader = make_shared<PixelShader>( "data/shaders/forwardTransparent.pso" );

	// Create fullscreen quad
	mQuadVertexShader = unique_ptr<VertexShader>( new VertexShader( "data/shaders/quad.vso" ));

	Vertex vertices[6];
	vertices[0] = Vertex( ruVector3( -0.5, -0.5, 0.0f ), ruVector2( 0, 0 ));
	vertices[1] = Vertex( ruVector3( pEngine->GetResolutionWidth() - 0.5, -0.5, 0.0f ), ruVector2( 1, 0 ));
	vertices[2] = Vertex( ruVector3( -0.5, pEngine->GetResolutionHeight() - 0.5, 0 ), ruVector2( 0, 1 ));
	vertices[3] = Vertex( ruVector3( pEngine->GetResolutionWidth() - 0.5, -0.5, 0.0f ), ruVector2( 1, 0 ));
	vertices[4] = Vertex( ruVector3( pEngine->GetResolutionWidth() - 0.5, pEngine->GetResolutionHeight() - 0.5, 0.0f ), ruVector2( 1, 1 ));
	vertices[5] = Vertex( ruVector3( -0.5, pEngine->GetResolutionHeight() - 0.5, 0.0f ), ruVector2( 0, 1 ));

	void * lockedData;
	pD3D->CreateVertexBuffer( 6 * sizeof( Vertex ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &mQuadVertexBuffer, 0 );
	mQuadVertexBuffer->Lock( 0, 0, &lockedData, 0 );
	memcpy( lockedData, vertices, sizeof( Vertex ) * 6 );
	mQuadVertexBuffer->Unlock( );

	D3DXMatrixOrthoOffCenterLH ( &mOrthoProjectionMatrix, 0, pEngine->GetResolutionWidth(), pEngine->GetResolutionHeight(), 0, 0, 1024 );
}

int Engine::CreateRenderWindow( int width, int height, int fullscreen ) {
    // get instance of this process
    HINSTANCE instance = GetModuleHandle ( 0 );
    // setup window class
    const wchar_t * className = L"Mine";
    WNDCLASSEXW wcx = { 0 };
    wcx.cbSize = sizeof ( wcx );
    wcx.hCursor = LoadCursor ( NULL, IDC_ARROW );
    wcx.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
    wcx.hInstance = instance;
    wcx.lpfnWndProc = WindowProcess;
    wcx.lpszClassName = className;
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClassExW ( &wcx );
    DWORD style;
    if ( !fullscreen ) {
        // windowed style prevent device lost
        style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    } else {
        // in fullscreen we has to deal with device lost
        style = WS_POPUP;
    }
    RECT wRect = { 0, 0, width, height };
    // make client region fits to the current resolution
    AdjustWindowRect ( &wRect, style, 0 );
    // create window
    mWindowHandle = CreateWindowW( className, className, style, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, instance, 0 );

    if( !mWindowHandle ) { // fail
        return 0;
    }
    // setup window
    ShowWindow ( mWindowHandle, SW_SHOW );
    UpdateWindow ( mWindowHandle );
    SetActiveWindow ( mWindowHandle );
    SetForegroundWindow ( mWindowHandle );
    // init input
    ruInput::Init( mWindowHandle );
    // success
    return 1;
}

void Engine::CreatePhysics() {
    Physics::CreateWorld();
}

void Engine::RenderWorld() {	
	if( pD3D->TestCooperativeLevel() == D3DERR_DEVICELOST ) {
		if( !mPaused ) {
			Log::Write( "Device lost. Engine paused!" );
		}
		mPaused = true;		
	}

	// window message pump
	UpdateMessagePump();
	
	if( mPaused ) {
		if( pD3D->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )  {
			OnLostDevice();
			mPaused = false;
			Log::Write( "Lost device handled. Engine restored!" );
		}
		return;
	}

	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();

	if( !camera || !mRunning ) {
        return;
    }
   
    // clear statistics
    mDIPCount = 0;
    mTextureChangeCount = 0;

    // precalculations
	auto & nodes = SceneFactory::GetNodeList();
    for( auto pWeak : nodes ) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if( node ) {        
			node->CheckFrustum( camera.get() );
		}
    }

	//mParallaxEnabled = true;
	//mHDREnabled = true;
	//mFXAAEnabled = true;

    // Begin rendering
	PrepareMeshMaps();

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
	GPURegisterStack gpuRegisterStack;

	pD3D->SetRenderState( D3DRS_ZENABLE, TRUE );
	pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	pD3D->SetTexture( 0, 0 );
	pD3D->SetTexture( 1, 0 );
	pD3D->SetTexture( 2, 0 );

	pD3D->SetRenderTarget( 0, mDepthSurface );
	pD3D->SetRenderTarget( 1, mNormalSurface );
	pD3D->SetRenderTarget( 2, mDiffuseSurface );

	pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

	if( mHDREnabled ) {
		pD3D->SetSamplerState( 0, D3DSAMP_SRGBTEXTURE, TRUE );
	}

	pD3D->SetVertexDeclaration( msVertexDeclaration );

	for( auto & texGroupPair : mDeferredMeshMap ) {
		IDirect3DTexture9 * pDiffuseTexture = texGroupPair.first;
		IDirect3DTexture9 * pNormalTexture = nullptr;

		// skip group, if it has no meshes
		if( texGroupPair.second.size() == 0 ) {
			continue;
		}

		pD3D->SetTexture( 0, pDiffuseTexture );

		for( auto weakMesh : texGroupPair.second ) {
			shared_ptr<Mesh> & pMesh = weakMesh.lock();

			if( !pMesh->IsHardwareBuffersGood() ) {
				continue;
			}
			if( pMesh->GetVertices().size() == 0 ) {
				continue;
			}

			if( pMesh->GetHeightTexture() ) {
				if( pMesh->IsSkinned() ) {
					mGBufferVertexShaderSkin->Bind();
					mGBufferPixelShaderSkin->Bind();
				} else if( IsParallaxEnabled() ) {
					pD3D->SetTexture( 2, pMesh->GetHeightTexture()->GetInterface() );
					mGBufferVertexShaderPOM->Bind();
					mGBufferPixelShaderPOM->Bind();
				} else {
					mGBufferVertexShader->Bind();
					mGBufferPixelShader->Bind();
				}
			} else {
				if( pMesh->IsSkinned() ) {
					mGBufferVertexShaderSkin->Bind();
					mGBufferPixelShaderSkin->Bind();
				} else {
					mGBufferVertexShader->Bind();
					mGBufferPixelShader->Bind();
				}
			}

			// prevent overhead with normal texture
			if( pMesh->GetNormalTexture() ) {
				IDirect3DTexture9 * meshNormalTexture = pMesh->GetNormalTexture()->GetInterface();
				if( meshNormalTexture != pNormalTexture ) {
					pD3D->SetTexture( 1, pMesh->GetNormalTexture()->GetInterface());
					pNormalTexture = meshNormalTexture;
				}
			}

			// render mesh
			if( camera ) {
				auto & owners = pMesh->GetOwners();

				for( auto weakOwner : owners ) {

					shared_ptr<SceneNode> & pOwner = weakOwner.lock();
						
					bool visible = true;
							
					// Bones is not renderable
					if( pOwner->IsBone() ) {
						visible = false; 
					} else {
						visible = pOwner->IsVisible();
					}

					if( visible && ( pOwner->IsInFrustum() || pOwner->IsSkinned() ) ) {
						camera->EnterDepthHack( pOwner->GetDepthHack() );
							
						D3DXMATRIX world, vwp;
						if( pMesh->IsSkinned() ) {
							D3DXMatrixIdentity( &world );
						} else {
							world = pOwner->GetWorldMatrix();
						}

						D3DXMatrixMultiply( &vwp, &world, &camera->mViewProjection );

						// Pass pixel shader constants to GPU by a single call	
						gpuRegisterStack.Clear();
						gpuRegisterStack.PushFloat( pOwner->GetAlbedo() );
						pD3D->SetPixelShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );
								
						// Pass vertex shader constants to GPU by a single call	
						gpuRegisterStack.Clear();
						gpuRegisterStack.PushMatrix( world );
						gpuRegisterStack.PushMatrix( vwp );
						gpuRegisterStack.PushVector( camera->GetPosition() );
						gpuRegisterStack.PushVector( pOwner->GetTexCoordFlow() );
						for( auto bone : pMesh->GetBones() ) {
							shared_ptr<SceneNode> boneNode = bone->mNode.lock();
							if( boneNode ) {
								gpuRegisterStack.PushMatrix( TransformToMatrix( boneNode->GetRelativeTransform() * pOwner->GetLocalTransform() ));
							}
						}						
						pD3D->SetVertexShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

						RenderMesh( pMesh );

						camera->LeaveDepthHack();						
					}
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
	pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pD3D->SetRenderState( D3DRS_STENCILENABLE, FALSE );

	if( camera ) {	
		// Begin light occlusion queries
		mSkyboxPixelShader->Bind();
		mSkyboxVertexShader->Bind();

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

							D3DXMATRIX wvp = SetUniformScaleTranslationMatrix( 1.25f * pLight->mRadius,  pLight->GetPosition() );
							D3DXMatrixMultiply( &wvp, &wvp, &camera->mViewProjection );
					
							gpuRegisterStack.Clear();
							gpuRegisterStack.PushMatrix( wvp );
							pD3D->SetVertexShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

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

		// Render Skybox
		if( camera->mSkybox ) {
			D3DXMATRIX wvp;
			D3DXMatrixTranslation( &wvp, camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z );
			D3DXMatrixMultiply( &wvp, &wvp, &camera->mViewProjection );
			
			gpuRegisterStack.Clear();
			gpuRegisterStack.PushMatrix( wvp );
			pD3D->SetVertexShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

			camera->mSkybox->Render( );	
		}  

		// Bind G-Buffer Textures
		pD3D->SetTexture( 0, mDepthMap );
		pD3D->SetTexture( 1, mNormalMap );
		pD3D->SetTexture( 2, mDiffuseMap );

		// Apply ambient lighting
		mAmbientPixelShader->Bind();

		gpuRegisterStack.Clear();
		gpuRegisterStack.PushVector( mAmbientColor );
		pD3D->SetPixelShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

		RenderFullscreenQuad();

		// Render point lights
		pD3D->SetRenderState( D3DRS_STENCILENABLE, TRUE );

		pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );		
		pD3D->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		
		for( auto lWeak : camera->GetNearestPathPoint()->GetListOfVisibleLights() ) {
			shared_ptr<PointLight> & pLight = lWeak.lock();
			if( pLight ) {
				if( pLight->IsVisible() ) {
					if( pLight->mInFrustum  ) {					
						pD3D->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
						pD3D->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
						pD3D->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );			

						pD3D->SetRenderState( D3DRS_ZENABLE, TRUE );
								
						// Setup point light pixel shader
						if( pLight->GetPointTexture() ) {
							mPointLightPixelShaderTexProj->Bind();
						} else {
							mPointLightPixelShader->Bind();
						}

						if( pLight->GetPointTexture() ) {
							pD3D->SetTexture( 3, pLight->GetPointTexture()->mCubeTexture );
						} 

						// Load pixel shader constants
						gpuRegisterStack.Clear();
						gpuRegisterStack.PushMatrix( camera->invViewProjection );
						gpuRegisterStack.PushVector( pLight->GetPosition() );
						gpuRegisterStack.PushVector( pLight->GetColor() );
						gpuRegisterStack.PushFloat( pLight->GetRange() );
						gpuRegisterStack.PushVector( camera->GetPosition() );
						pD3D->SetPixelShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

						// Render bounding sphere
						D3DXMATRIX wvp = SetUniformScaleTranslationMatrix( 1.5f * pLight->mRadius, pLight->GetPosition() );
						D3DXMatrixMultiply( &wvp, &wvp, &camera->mViewProjection );
							
						pD3D->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 );

						mBoundingSphere->DrawSubset( 0 );
						pD3D->SetVertexDeclaration( msVertexDeclaration );

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
		auto & spotLights = SceneFactory::GetSpotLightList();
		for( auto & lWeak : spotLights ) {
			shared_ptr<SpotLight> & pLight = lWeak.lock();
			if( pLight ) {
				if( camera->mFrustum.IsSphereInside( pLight->GetPosition(), pLight->GetRange() ) && pLight->IsVisible()  ) {
					// If shadows enabled, render shadowmap firts
					if( IsSpotLightShadowsEnabled() ) {
						pD3D->SetRenderState( D3DRS_STENCILENABLE, FALSE );
						pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
						pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
						
						pD3D->SetTexture( 4, nullptr );

						pD3D->SetRenderTarget( 0, mShadowMapSurface );
						pD3D->SetDepthStencilSurface( mDepthStencilSurface );
						pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

						mSpotLightShadowMapPixelShader->Bind();
						mSpotLightShadowMapVertexShader->Bind();

						pLight->BuildSpotProjectionMatrixAndFrustum();
						
						for( auto & texGroupPair : mDeferredMeshMap ) {
							auto & group = texGroupPair.second;
							for( auto & weakMesh : group ) {
								shared_ptr<Mesh> mesh = weakMesh.lock();
								auto & owners = mesh->GetOwners();
								for( auto & weakOwner : owners ) {
									shared_ptr<SceneNode> & pOwner = weakOwner.lock();
									if( pOwner->IsVisible() && pLight->GetFrustum().IsAABBInside( mesh->GetBoundingBox(), pOwner->GetPosition())) {
										D3DXMATRIX world, wvp;
										world = pOwner->GetWorldMatrix();
										D3DXMatrixMultiply( &wvp, &world, &pLight->GetViewProjectionMatrix() );

										gpuRegisterStack.Clear();
										gpuRegisterStack.PushMatrix( wvp );
										pD3D->SetVertexShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

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

						pD3D->SetTexture( 4, mShadowMap );
					}

					pD3D->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
					pD3D->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
					pD3D->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

					// Render Oriented Bounding Cone
					float height = pLight->GetRange() * 2.5;
					float radius = height * sinf( ( pLight->GetOuterAngle() * 0.75f ) * SIMD_PI / 180.0f );
					D3DXMATRIX scale, world, wvp;
					D3DXMatrixScaling( &scale, radius, height, radius );
					world = pLight->GetWorldMatrix();
					D3DXMatrixMultiply( &world, &scale, &world );
					D3DXMatrixMultiply( &wvp, &world, &camera->mViewProjection );
					pD3D->SetVertexShaderConstantF( 0, &wvp.m[0][0], 4 );

					mBoundingCone->DrawSubset( 0 );
					pD3D->SetVertexDeclaration( msVertexDeclaration );
					
					// Setup pixel shader
					if( IsSpotLightShadowsEnabled() ) {		
						mSpotLightPixelShaderShadows->Bind();		
					} else {
						mSpotLightPixelShader->Bind();
					}

					pD3D->SetTexture( 3, pLight->GetSpotTexture()->GetInterface() );

					pLight->BuildSpotProjectionMatrixAndFrustum();

					// Load pixel shader constants
					gpuRegisterStack.Clear();
					gpuRegisterStack.PushMatrix( camera->invViewProjection );
					gpuRegisterStack.PushMatrix( pLight->GetViewProjectionMatrix() );
					gpuRegisterStack.PushVector( pLight->GetPosition() );
					gpuRegisterStack.PushVector( pLight->GetColor() );
					gpuRegisterStack.PushFloat( pLight->GetRange() );
					pD3D->SetPixelShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

					pD3D->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
					pD3D->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
					pD3D->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );

					// Apply lighting to pixels, that marked by bounding cone
					RenderFullscreenQuad();
				}
			}
		}
	}

	// Apply post-effects (HDR, FXAA, and so on)
	IDirect3DTexture9 * finalFrame = mFrame[0];

	pD3D->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	pD3D->SetRenderState( D3DRS_ZENABLE, FALSE );
	pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	SetGenericSamplersFiltration( D3DTEXF_POINT, true );

	// Do HDR
	if( mHDREnabled ) {	

		// Calculate HDR-frame luminance
		pD3D->SetTexture( 7, mHDRFrame );
		pD3D->SetRenderTarget( 0, mScaledSceneSurf );
		pD3D->Clear( 0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

		mScaleScenePixelShader->Bind();
		RenderFullscreenQuad();

		mDownScalePixelShader->Bind();

		pD3D->SetTexture( 7, mScaledScene );
		for( int i = 0; i < mHDRDownSampleCount; i++ ) {
			float pixelSize = 1.0f / static_cast<float>( IntegerPow( 2, mHDRDownSampleCount - i ));	

			pD3D->SetRenderTarget( 0, mDownSampSurf[ i ] );
			pD3D->Clear( 0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

			gpuRegisterStack.Clear();
			gpuRegisterStack.PushFloat( pixelSize );
			pD3D->SetPixelShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

			RenderFullscreenQuad();

			pD3D->SetTexture( 7, mDownSampTex[ i ] );
		}

		for( int i = 0; i < 8; i++ ) {
			pD3D->SetTexture( i, nullptr );
		}

		// now we get average frame luminance presented as 1x1 pixel RGBA8 texture
		// render it into R32F luminance texture
		IDirect3DTexture9 * pTexSwap = mAdaptedLuminanceLast;
		mAdaptedLuminanceLast = mAdaptedLuminanceCurrent;
		mAdaptedLuminanceCurrent = pTexSwap;

		IDirect3DSurface9 * pSurfAdaptedLum = NULL;
		mAdaptedLuminanceCurrent->GetSurfaceLevel( 0, &pSurfAdaptedLum );

		pD3D->SetRenderTarget( 0, pSurfAdaptedLum );
		pD3D->SetTexture( 6, mAdaptedLuminanceLast );
		pD3D->SetTexture( 7, mDownSampTex[ mHDRDownSampleCount - 1 ] );

		mAdaptationPixelShader->Bind();

		// Set pixel shader constants
		gpuRegisterStack.Clear();
		gpuRegisterStack.PushFloat( 0.75f );
		pD3D->SetPixelShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

		RenderFullscreenQuad();

		pSurfAdaptedLum->Release();

		// And finally do tone-mapping
		pD3D->SetRenderTarget( 0, mFrameSurface[0] );
		pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

		pD3D->SetTexture( 0, mHDRFrame );
		pD3D->SetTexture( 7, mAdaptedLuminanceCurrent );

		mToneMapShader->Bind();
		RenderFullscreenQuad();

		finalFrame = mFrame[0];
	};
	
	// Do FXAA
	if( mFXAAEnabled ) {
		pD3D->SetRenderTarget( 0, mFrameSurface[1] );
		pD3D->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

		pD3D->SetTexture( 0, finalFrame );

		mFXAAPixelShader->Bind();

		gpuRegisterStack.Clear();
		gpuRegisterStack.PushFloat( GetResolutionWidth(), GetResolutionHeight() );
		pD3D->SetPixelShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

		RenderFullscreenQuad();

		finalFrame = mFrame[1];
	}

	// Postprocessing
	if( mHDREnabled ) {
		pD3D->SetRenderState( D3DRS_SRGBWRITEENABLE, TRUE );
	}

	pD3D->SetRenderTarget( 0, mpBackBuffer );
	pD3D->Clear( 0, 0, D3DCLEAR_TARGET , D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

	pD3D->SetTexture( 0, finalFrame );

	mSkyboxPixelShader->Bind();
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
	pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

	pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pD3D->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	mTransparentPixelShader->Bind();
	mTransparentVertexShader->Bind();

	for( auto & texGroupPair : mForwardMeshMap ) {
		IDirect3DTexture9 * diffuseTexture = texGroupPair.first;

		auto & meshGroup = texGroupPair.second;

		if( meshGroup.size() == 0 ) {
			continue;
		}

		pD3D->SetTexture( 0, diffuseTexture );

		for( auto & weakMesh : meshGroup ) {
			shared_ptr<Mesh> pMesh = weakMesh.lock();

			auto & owners = pMesh->GetOwners();
			for( auto & weakOwner : owners ) {
				shared_ptr<SceneNode> pOwner = weakOwner.lock();
					
				if( pOwner->IsVisible() ) {
					D3DXMATRIX world, wvp;
					world = pOwner->GetWorldMatrix();
					D3DXMatrixMultiply( &wvp, &world, &camera->mViewProjection );					

					gpuRegisterStack.Clear();
					gpuRegisterStack.PushMatrix( wvp );
					pD3D->SetVertexShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );

					gpuRegisterStack.Clear();
					gpuRegisterStack.PushFloat( pMesh->GetOpacity() );
					pD3D->SetPixelShaderConstantF( 0, gpuRegisterStack.GetPointer(), gpuRegisterStack.mRegisterCount );
					
					RenderMesh( pMesh );
				}
			}
		}						
	}

    mpParticleSystemRenderer->Render();

    // render gui on top of all    
    mpGUIRenderer->Render();

    // end rendering
    pD3D->EndScene();
    pD3D->Present( 0, 0, 0, 0 );

    // update sound subsystem
    pfSystemUpdate();
}

void Engine::UpdateMessagePump() {
    MSG message;
    while ( PeekMessage ( &message, NULL, 0, 0, PM_REMOVE ) ) {
        DispatchMessage ( &message );
        if ( message.message == WM_QUIT ) {
            Shutdown();
        }
    }
}



void Engine::OnLostDevice() {
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

void Engine::Reset() {
	mpDevice->Reset( &mPresentParameters );
	SetDefaults();
	OnResetDevice();
}

void Engine::ChangeVideomode( int width, int height, bool fullscreen, bool vsync ) {
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

void Engine::OnResetDevice() {
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

bool Engine::IsTextureFormatOk( D3DFORMAT TextureFormat ) {
	return SUCCEEDED( mpDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_TEXTURE, TextureFormat) );
}

void Engine::SetSpotLightShadowMapSize( int size ) {
	
}

int Engine::GetDIPCount() {
	return mDIPCount;
}

float Engine::GetResolutionHeight() {
	return mResHeight;
}

float Engine::GetResolutionWidth() {
	return mResWidth;
}

void Engine::RegisterDIP() {
	mDIPCount++;
}

shared_ptr<TextRenderer> & Engine::GetTextRenderer() {
	return mpTextRenderer;
}

bool Engine::IsSpotLightShadowsEnabled() {
	return mUseSpotLightShadows;
}

bool Engine::IsPointLightShadowsEnabled() {
	return mUsePointLightShadows;
}

void Engine::SetSpotLightShadowsEnabled( bool state ) {
	mUseSpotLightShadows = state;
}

void Engine::SetPointLightShadowsEnabled( bool state ) {
	mUsePointLightShadows = state;
}

void Engine::SetAmbientColor( ruVector3 ambColor ) {
	mAmbientColor = ambColor;
}

ruVector3 Engine::GetAmbientColor() {
	return mAmbientColor;
}

void Engine::SetHDREnabled( bool state ) {
	mHDREnabled = state;
}

bool Engine::IsHDREnabled() {
	return mHDREnabled;
}

void Engine::SetFXAAEnabled( bool state ) {
	mFXAAEnabled = state;
}

float Engine::GetGUIHeightScaleFactor() const
{
	return mResHeight / ruVirtualScreenHeight;
}

bool Engine::IsFXAAEnabled() {
	return mFXAAEnabled;
}

void Engine::Shutdown() {
	mRunning = false;
}

shared_ptr<Cursor> & Engine::GetCursor()
{
	return mCursor;
}

bool Engine::IsNonPowerOfTwoTexturesSupport() {
	D3DCAPS9 caps;
	pD3D->GetDeviceCaps( &caps );
	char npotcond = caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL;
	char pot = caps.TextureCaps & D3DPTEXTURECAPS_POW2;
	return !(npotcond || pot);
}

int Engine::GetTextureChangeCount() {
	return mTextureChangeCount;
}

void Engine::SetTextureStoragePath( const string & path ) {
	mTextureStoragePath = path;
}

std::string Engine::GetTextureStoragePath() {
	return mTextureStoragePath;
}

void Engine::SetGenericSamplersFiltration( D3DTEXTUREFILTERTYPE filter, bool disableMips ) {
	// number of generic samplers (i.e. for diffuse, normal and height textures )
	const int genericSamplersCount = 3;

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

void Engine::SetAnisotropicTextureFiltration( bool state ) {
	mAnisotropicFiltering = state;
}

bool Engine::IsAnisotropicFilteringEnabled() {
	return mAnisotropicFiltering;
}

void Engine::SetDefaults() {
	pD3D->SetRenderState ( D3DRS_LIGHTING, FALSE );
	pD3D->SetRenderState( D3DRS_ZENABLE, TRUE );
	pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
	pD3D->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	pD3D->SetRenderState ( D3DRS_ALPHAREF, 10 );
	pD3D->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pD3D->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
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

	pD3D->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pD3D->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

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

static void PrepareMeshMap( MeshMap & mm ) {
	for( auto texGroupPairIter = mm.begin(); texGroupPairIter != mm.end();  ) {
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
			texGroupPairIter = mm.erase( texGroupPairIter );
		}
	}
}

void Engine::PrepareMeshMaps() {
	PrepareMeshMap( mDeferredMeshMap );
	PrepareMeshMap( mForwardMeshMap );
}

void Engine::RenderMesh( const shared_ptr<Mesh> & mesh ) {
	pD3D->SetStreamSource( 0, mesh->mVertexBuffer, 0, sizeof( Vertex ));
	pD3D->SetIndices( mesh->mIndexBuffer );

	mpDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, mesh->mVertices.size(), 0, mesh->mTriangles.size() );
	++mDIPCount;
}

float Engine::GetGUIWidthScaleFactor() const {
	return mResWidth / ruVirtualScreenWidth;
}

void Engine::SetParallaxEnabled( bool state ) {
	mParallaxEnabled = state;
}

void Engine::AddMesh( const shared_ptr<Mesh> & mesh ) {
	mesh->CalculateAABB();
	if( mesh->GetOpacity() > 0.99f ) { 
		// pass it to deferred renderer
		if( mesh->GetDiffuseTexture() ) {
			auto textureGroup = mDeferredMeshMap.find( mesh->GetDiffuseTexture()->GetInterface() );

			if( textureGroup == mDeferredMeshMap.end()) {
				mDeferredMeshMap[ mesh->GetDiffuseTexture()->GetInterface() ] = vector<weak_ptr<Mesh>>();
			}

			mDeferredMeshMap[ mesh->GetDiffuseTexture()->GetInterface() ].push_back( mesh );
		}
	} else { 
		// pass it to forward renderer
		if( mesh->GetDiffuseTexture() ) {
			mForwardMeshMap[ mesh->GetDiffuseTexture()->GetInterface() ].push_back( mesh );
		}
	}
}

bool Engine::IsParallaxEnabled() {
	return mParallaxEnabled;
}

void Engine::SetCursorVisible( bool state ) {
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

void Engine::SetCursor( shared_ptr<ruTexture> texture, int w, int h ) {
	mCursor = shared_ptr<Cursor>( new Cursor( w, h, std::dynamic_pointer_cast<Texture>( texture )));
}
