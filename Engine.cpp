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
#include "MultipleRTDeferredRenderer.h"
#include "FXAA.h"
#include "FPSCounter.h"
#include "ForwardRenderer.h"
#include "BitmapFont.h"
#include "TextRenderer.h"


Engine::Engine() : mpDevice( nullptr ), mpDirect3D( nullptr ), mpDeferredRenderer( nullptr ), 
	mpForwardRenderer( nullptr ), mpParticleSystemRenderer( nullptr ), mpTextRenderer( nullptr ),
	mpGUIRenderer( nullptr ), mAmbientColor( 0.05, 0.05, 0.05 ), mUsePointLightShadows( false ),
	mUseSpotLightShadows( false ), mHDREnabled( false ), mRunning( true ), mFXAAEnabled( false ),
	mTextureStoragePath( "data/textures/generic/" ), mParallaxEnabled( true ) {

}

Engine::~Engine() {	
    while( BitmapFont::fonts.size() ) {
        delete BitmapFont::fonts.front();
    }
    for( auto tmr : Timer::timers ) {
        delete tmr;
    }
    for( auto & kv : CubeTexture::all ) {
        delete kv.second;
    }

    //delete mpTextRenderer;
    delete mpParticleSystemRenderer;
    delete mpDeferredRenderer;
    delete mpGUIRenderer;
    while( GUINode::msNodeList.size() ) {
        delete GUINode::msNodeList.front();
    }
    while( SceneNode::msNodeList.size() ) {
        delete SceneNode::msNodeList.front();
    }
	Mesh::CleanUp();
    if( mpForwardRenderer ) {
        delete mpForwardRenderer;
    }
    Texture::DeleteAll();
    
    int counter = 0;
    if( Engine::Instance().GetDevice() ) {
        while( Engine::Instance().GetDevice()->Release() ) {
            counter++;
        }
    }
    if( mpDirect3D ) {
        while( mpDirect3D->Release() ) {
            counter++;
        }
    }
    Physics::DestructWorld();
    pfSystemDestroy();
}

void Engine::Initialize( int width, int height, int fullscreen, char vSync ) {
    if ( width == 0 || height == 0 ) {
        width = GetSystemMetrics ( SM_CXSCREEN );
		height = GetSystemMetrics ( SM_CYSCREEN );
		Log::Write( "0 passed as resolution, using desktop native resolution!" );
    }
  
    if( !CreateRenderWindow( width, height, fullscreen )) {
		Log::Error( "Failed to create render window! Engine initialization failed!" );
    }

    // try to create Direct3D9
    mpDirect3D = Direct3DCreate9( D3D_SDK_VERSION );

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

	mNativeResolutionWidth = GetSystemMetrics ( SM_CXSCREEN );;
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

	GetDevice()->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &mpBackBuffer );

    SetDefaults();

    CreatePhysics( );

	pfSystemCreateLogFile( "ProjectF.log" ); 

	pfSystemEnableMessagesOutputToConsole(); 
	pfSystemEnableMessagesOutputToLogFile(); 

    pfSystemInit( );
    pfSetListenerDopplerFactor( 0 );

    mpDeferredRenderer = new MultipleRTDeferredRenderer();	
    mpForwardRenderer = new ForwardRenderer();	
    mpParticleSystemRenderer = new ParticleSystemRenderer();
    mpTextRenderer = new TextRenderer();
    mpGUIRenderer = new GUIRenderer();
	mAmbientColor = ruVector3( 0.05, 0.05, 0.05 );
	mUsePointLightShadows = false;
	mUseSpotLightShadows = false;
	mHDREnabled = false;
	mRunning = true;
	mFXAAEnabled = false;
	mPaused = false;
	mChangeVideomode = false;
	mTextureStoragePath = "data/textures/generic/";
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
    ruInputInit( &mWindowHandle );
    // success
    return 1;
}

void Engine::CreatePhysics() {
    Physics::CreateWorld();
}

void Engine::RenderWorld() {
	
	if( GetDevice()->TestCooperativeLevel() == D3DERR_DEVICELOST ) {
		if( !mPaused ) {
			Log::Write( "Device lost. Engine paused!" );
		}
		mPaused = true;		
	}
	 // window message pump
	UpdateMessagePump();
	
	if( mPaused ) {
		if( GetDevice()->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )  {
			OnLostDevice();
			mPaused = false;
			Log::Write( "Lost device handled. Engine restored!" );
		}
		return;
	}
    if( !mRunning ) {
        return;
    }
    if( !Camera::msCurrentCamera ) {
        return;
    }
    mFPSCounter.RegisterFrame();
    // erase marked nodes
	Mesh::EraseOrphanMeshes();
    SceneNode::EraseUnusedNodes();
   
    // clear statistics
    mDIPCount = 0;
    mTextureChangeCount = 0;

    // precalculations
    for( auto node : SceneNode::msNodeList ) {
        // skip frustum flag, it will be set to true, if one of node's mesh are in frustum
        node->mInFrustum = false;
    }
    // begin dx scene
    GetDevice()->BeginScene();
    // begin rendering into G-Buffer
    GetDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
    SetZWriteEnabled( true );
    GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    SetAlphaBlendEnabled( false );
    mpDeferredRenderer->BeginFirstPass();
    // render from current camera
    RenderMeshesIntoGBuffer();
    // end render into G-Buffer and do a lighting passes
    if( IsHDREnabled() ) {
        GetDevice()->SetRenderState( D3DRS_SRGBWRITEENABLE, TRUE );
        GetDevice()->SetSamplerState( 2, D3DSAMP_SRGBTEXTURE, TRUE );
    } else {
        GetDevice()->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
        GetDevice()->SetSamplerState( 2, D3DSAMP_SRGBTEXTURE, FALSE );
    }
	SetAlphaBlendEnabled( false );
	GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );
    GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    SetZWriteEnabled( false );
    GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    SetStencilEnabled( false );
    mpDeferredRenderer->EndFirstPassAndDoSecondPass();
    // render all opacity meshes with forward renderer
    GetDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
	SetZWriteEnabled( true );
    GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    mpForwardRenderer->RenderMeshes();
	SetZWriteEnabled( false );
    // render particles after all, because deferred shading doesnt support transparency
    GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    SetStencilEnabled( false );
    mpParticleSystemRenderer->RenderAllParticleSystems();
    // render gui on top of all
    GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );
    mpGUIRenderer->RenderAllGUIElements();
    // finalize
    GetDevice()->EndScene();
    GetDevice()->Present( 0, 0, 0, 0 );
    // update sound subsystem
    pfSystemUpdate();
}

void ruUpdatePhysics( float timeStep, int subSteps, float fixedTimeStep ) {
    Physics::mpDynamicsWorld->stepSimulation( timeStep, subSteps, fixedTimeStep );
	// grab info about node's physic contacts
	SceneNode::UpdateContacts( );
}

void Engine::RenderMeshesIntoGBuffer() {
    for( auto groupIterator : Mesh::msMeshList ) {
        IDirect3DTexture9 * pDiffuseTexture = groupIterator.first;
        IDirect3DTexture9 * pNormalTexture = nullptr;
        auto & meshes = groupIterator.second;
        // skip group if it has no meshes
        if( meshes.size() == 0 ) {
            continue;
        }
        // bind diffuse texture
        mpDevice->SetTexture( 0, pDiffuseTexture );
        // each group has same texture
        mTextureChangeCount++;
        for( auto pMesh : meshes ) {
			if( !pMesh->mIndexBuffer || !pMesh->mVertexBuffer ) {
				continue;
			}
			if( pMesh->mVertices.size() == 0 ) {
				continue;
			}
			// bind height texture for parallax mapping
			if( pMesh->mHeightTexture && !pMesh->mSkinned ) {
				if( mParallaxEnabled ) {
					pMesh->mHeightTexture->Bind( 2 );
					mpDeferredRenderer->BindParallaxShaders();
				} else {
					mpDeferredRenderer->BindGenericShaders();
				}
			} else {
				if( pMesh->mSkinned ) {
					mpDeferredRenderer->BindGenericSkinShaders();
				} else {
					mpDeferredRenderer->BindGenericShaders();
				}
			}
            // prevent overhead with normal texture
            if( pMesh->GetNormalTexture() ) {
                IDirect3DTexture9 * meshNormalTexture = pMesh->GetNormalTexture()->GetInterface();
                if( meshNormalTexture != pNormalTexture ) {
                    mTextureChangeCount++;
                    pMesh->GetNormalTexture()->Bind( 1 );
                    pNormalTexture = meshNormalTexture;
                }
            }
            mpDeferredRenderer->RenderMesh( pMesh );            
        }
    }
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

LRESULT CALLBACK Engine::WindowProcess( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
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

void Engine::SetPixelShaderInt( UINT startRegister, int v ) {
    int buffer[ 4 ] = { v, 0, 0, 0 };
    GetDevice()->SetPixelShaderConstantI( startRegister, buffer, 1 );
}

void Engine::SetPixelShaderFloat( UINT startRegister, float v ) {
    float buffer[ 4 ] = { v, 0.0f, 0.0f, 0.0f };
    GetDevice()->SetPixelShaderConstantF( startRegister, buffer, 1 );
}

void Engine::SetPixelShaderFloat3( UINT startRegister, float * v ) {
    float buffer[ 4 ] = { v[0], v[1], v[2], 0.0f };
    GetDevice()->SetPixelShaderConstantF( startRegister, buffer, 1 );
}

void Engine::SetPixelShaderFloat3( UINT startRegister, float x, float y, float z ) {
	float buffer[ 4 ] = { x, y, z, 0.0f };
	GetDevice()->SetPixelShaderConstantF( startRegister, buffer, 1 );
}

void Engine::SetPixelShaderMatrix( UINT startRegister, D3DMATRIX * matrix ) {
    GetDevice()->SetPixelShaderConstantF( startRegister, &matrix->m[0][0], 4 );
}

void Engine::SetVertexShaderInt( UINT startRegister, int v ) {
    int buffer[ 4 ] = { v, v, v, v };
    GetDevice()->SetVertexShaderConstantI( startRegister, buffer, 1 );
}

void Engine::SetVertexShaderFloat( UINT startRegister, float v ) {
    float buffer[ 4 ] = { v, v, v, v };
    GetDevice()->SetVertexShaderConstantF( startRegister, buffer, 1 );
}

void Engine::SetVertexShaderFloat3( UINT startRegister, float * v ) {
    float buffer[ 4 ] = { v[0], v[1], v[2], v[2] };
    GetDevice()->SetVertexShaderConstantF( startRegister, buffer, 1 );
}

void Engine::SetVertexShaderMatrix( UINT startRegister, D3DMATRIX * matrix ) {
    GetDevice()->SetVertexShaderConstantF( startRegister, &matrix->m[0][0], 4 );
}

void Engine::OnLostDevice() {
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
	GetDevice()->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &mpBackBuffer );

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



Engine & Engine::Instance() {
	static Engine instance;
	return instance;
}

bool Engine::IsTextureFormatOk( D3DFORMAT TextureFormat ) {
	return SUCCEEDED( mpDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_TEXTURE, TextureFormat) );
}

IDirect3DDevice9 * Engine::GetDevice() {
	return mpDevice;
}

void Engine::SetSpotLightShadowMapSize( int size ) {
	mpDeferredRenderer->SetSpotLightShadowMapSize( size );
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

ForwardRenderer * Engine::GetForwardRenderer() {
	return mpForwardRenderer;
}

DeferredRenderer * Engine::GetDeferredRenderer() {
	return mpDeferredRenderer;
}

TextRenderer * Engine::GetTextRenderer() {
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

bool Engine::IsFXAAEnabled() {
	return mFXAAEnabled;
}

void Engine::Shutdown() {
	mRunning = false;
}

bool Engine::IsNonPowerOfTwoTexturesSupport() {
	D3DCAPS9 caps;
	Engine::Instance().GetDevice()->GetDeviceCaps( &caps );
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
			GetDevice()->SetSamplerState ( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			GetDevice()->SetSamplerState ( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
	} else if( filter == D3DTEXF_LINEAR ) {
		for( int i = 0; i < genericSamplersCount; i++ ) {
			GetDevice()->SetSamplerState ( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			GetDevice()->SetSamplerState ( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
	} else if( filter == D3DTEXF_ANISOTROPIC ) {
		GetDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
		GetDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		// it's too expensive to set anisotropic filtration to normal and height maps, so set linear
		for( int i = 1; i < genericSamplersCount; i++ ) {
			GetDevice()->SetSamplerState ( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			GetDevice()->SetSamplerState ( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
	}

	// mip filters
	if( filter == D3DTEXF_NONE || disableMips ) {
		for( int i = 0; i < genericSamplersCount; i++ ) {
			GetDevice()->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		}
	} else if( filter == D3DTEXF_POINT ) {
		for( int i = 0; i < genericSamplersCount; i++ ) {
			GetDevice()->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
		}
	} else if( filter == D3DTEXF_LINEAR || filter == D3DTEXF_ANISOTROPIC ) {
		for( int i = 0; i < genericSamplersCount; i++ ) {
			GetDevice()->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		}
	}
}

void Engine::SetAnisotropicTextureFiltration( bool state ) {
	mAnisotropicFiltering = state;
}

bool Engine::IsAnisotropicFilteringEnabled() {
	return mAnisotropicFiltering;
}


IDirect3DSurface9 * Engine::GetBackBuffer() {
	return mpBackBuffer;
}

void Engine::Continue() {
	mPaused = false;
}

void Engine::Pause() {
	mPaused = true;
}

void Engine::SetDefaults() {
	GetDevice()->SetRenderState ( D3DRS_LIGHTING, FALSE );
	GetDevice()->SetRenderState ( D3DRS_ZENABLE, TRUE );
	GetDevice()->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
	GetDevice()->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	GetDevice()->SetRenderState ( D3DRS_ALPHAREF, 10 );
	GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	GetDevice()->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	GetDevice()->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );

	GetDevice()->SetRenderState( D3DRS_STENCILREF, 0x0 );
	GetDevice()->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
	GetDevice()->SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
	GetDevice()->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE );
	GetDevice()->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
	GetDevice()->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR );
	GetDevice()->SetRenderState( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_INCR );

	// setup samplers
	GetDevice()->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	GetDevice()->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	GetDevice()->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	GetDevice()->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );    

	GetDevice()->SetSamplerState ( 3, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	GetDevice()->SetSamplerState ( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );   

	GetDevice()->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	GetDevice()->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	GetDevice()->SetSamplerState ( 3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	GetDevice()->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	GetDevice()->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	D3DCAPS9 dCaps;
	mpDirect3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &dCaps );

	GetDevice()->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy );
	GetDevice()->SetSamplerState ( 1, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy );
	GetDevice()->SetSamplerState ( 2, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy );
	GetDevice()->SetSamplerState ( 3, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy );

	SetAnisotropicTextureFiltration( true );

	SetParallaxEnabled( true );
}

void Engine::DrawIndexedTriangleList( int vertexCount, int faceCount ) {
	mpDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, faceCount );
	++mDIPCount;
}

void Engine::SetVertexShaderVector3( UINT startRegister, ruVector3 v ) {
	float buffer[ 4 ] = { v.x, v.y, v.z, 0.0f };
	GetDevice()->SetVertexShaderConstantF( startRegister, buffer, 1 );
}

void Engine::SetAlphaBlendEnabled( bool state ) {
	mpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, state );
}

void Engine::SetZWriteEnabled( bool state ) {
	mpDevice->SetRenderState( D3DRS_ZWRITEENABLE, state );
}

void Engine::SetStencilEnabled( bool state ) {
	mpDevice->SetRenderState( D3DRS_STENCILENABLE, state );
}
