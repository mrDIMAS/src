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

void CheckDXErrorFatalFunc( HRESULT errCode, const string & file, int line ) {
	if( FAILED( errCode )) {
		string message = (string)(StringBuilder( "DirectX 9 Error. Code: " ) << errCode << "\nError: " << DXGetErrorString( errCode ) << "\nDescription: " << DXGetErrorDescription( errCode ) << "\nFile: " << file << "\nLine: " << line);
		Log::Error( message );
	}
}


Engine::~Engine() {	
    for( auto fnt : BitmapFont::fonts ) {
        delete fnt;
    }
    for( auto tmr : Timer::timers ) {
        delete tmr;
    }
    for( auto & kv : CubeTexture::all ) {
        delete kv.second;
    }
   
    delete mpTextRenderer;
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
    if ( width == 0 ) {
        width = GetSystemMetrics ( SM_CXSCREEN );
    }
    if ( height == 0 ) {
        height = GetSystemMetrics ( SM_CYSCREEN );
    }
    if( !CreateRenderWindow( width, height, fullscreen )) {
        return;
    }

    // try to create Direct3D9
    mpDirect3D = Direct3DCreate9( D3D_SDK_VERSION );

    // epic fail
    if( !mpDirect3D ) {
		Log::Error( "Failed to Direct3DCreate9! Ensure, that you have latest video drivers! Engine initialization failed!" );
    }

    // check d3d caps, to ensure, that user have modern hardware
    D3DCAPS9 dCaps;
    CheckDXErrorFatal( mpDirect3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &dCaps ));

    unsigned char psVerHi = D3DSHADER_VERSION_MAJOR( dCaps.PixelShaderVersion );
    unsigned char psVerLo = D3DSHADER_VERSION_MINOR( dCaps.PixelShaderVersion );

    // epic fail
    if( psVerHi < 2 ) {
		mpDirect3D->Release();
        Log::Error( "Your graphics card doesn't support Pixel Shader 2.0. Engine initialization failed! Buy a modern video card!" );        
    }

    mResWidth = width;
    mResHeight = height;

    D3DDISPLAYMODE displayMode = { 0 };
    mpDirect3D->GetAdapterDisplayMode ( D3DADAPTER_DEFAULT, &displayMode );

    // present parameters
    D3DPRESENT_PARAMETERS presentParameters = { 0 };
    presentParameters.BackBufferCount = 2;
    presentParameters.EnableAutoDepthStencil = TRUE;
    if( vSync ) {
        presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    } else {
        presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    presentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
    presentParameters.BackBufferWidth = width;
    presentParameters.BackBufferHeight = height;

    if ( fullscreen ) {
        presentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
        presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        presentParameters.Windowed = FALSE;
        presentParameters.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
    } else {
        presentParameters.BackBufferFormat = displayMode.Format;
        presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        presentParameters.Windowed = TRUE;
    }

    // no multisampling, because of deferred shading
    presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
    presentParameters.MultiSampleQuality = 0;

    // create device
    if( FAILED( mpDirect3D->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE, &presentParameters, &mpDevice ))) {
		mpDirect3D->Release();
		Log::Error( "Engine initialization failed! Buy a modern video card!" );	
	}

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
    GetDevice()->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );

    GetDevice()->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
    GetDevice()->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );    

	GetDevice()->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	GetDevice()->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

    GetDevice()->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    GetDevice()->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	GetDevice()->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy );
	GetDevice()->SetSamplerState ( 1, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy );

	SetAnisotropicTextureFiltration( true );

    CreatePhysics( );
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
    window = CreateWindowW( className, className, style, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, instance, 0 );

    if( !window ) { // fail
        return 0;
    }
    // setup window
    ShowWindow ( window, SW_SHOW );
    UpdateWindow ( window );
    SetActiveWindow ( window );
    SetForegroundWindow ( window );
    // init input
    ruInputInit( &window );
    // success
    return 1;
}

void Engine::CreatePhysics() {
    Physics::CreateWorld();
}

void Engine::RenderWorld() {
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
    // window message pump
    UpdateMessagePump();
    // clear statistics
    mDIPCount = 0;
    mTextureChangeCount = 0;
    // build view and projection matrices, frustum, also attach sound listener to camera
    Camera::msCurrentCamera->Update();
    // precalculations
    for( auto node : SceneNode::msNodeList ) {
        node->CalculateGlobalTransform();
        node->PerformAnimation();
        // update all sounds attached to node, and physical interaction sounds( roll, hit )
        node->UpdateSounds();
        // skip frustum flag, it will be set to true, if one of node's mesh
        // are in frustum
        node->mInFrustum = false;
    }
    // update lights
    for( auto light : Light::msSpotLightList ) {
        light->DoFloating();
    }
    for( auto light : Light::msPointLightList ) {
        light->DoFloating();
    }
    // begin dx scene
    CheckDXErrorFatal( GetDevice()->BeginScene());
    // begin rendering into G-Buffer
    GetDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
    GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
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
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );
    GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    GetDevice()->SetRenderState( D3DRS_STENCILENABLE, FALSE );
    mpDeferredRenderer->EndFirstPassAndDoSecondPass();
    // render all opacity meshes with forward renderer
    GetDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
	GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    mpForwardRenderer->RenderMeshes();
	GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    // render particles after all, because deferred shading doesnt support transparency
    GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    GetDevice()->SetRenderState( D3DRS_STENCILENABLE, FALSE );

    mpParticleSystemRenderer->RenderAllParticleSystems();
    // render gui on top of all
    GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );
    mpGUIRenderer->RenderAllGUIElements();
    // render light flares without writing to z-buffer
    //Light::RenderLightFlares();
    // finalize
    GetDevice()->EndScene();
    if( GetDevice()->Present( 0, 0, 0, 0 ) == D3DERR_DEVICELOST ) {
		mpDeferredRenderer->OnDeviceLost();
		OnDeviceLost();
	}
    // grab info about node's physic contacts
    SceneNode::UpdateContacts( );
    // update sound subsystem
    pfSystemUpdate();
}

void ruUpdatePhysics( float timeStep, int subSteps, float fixedTimeStep ) {
    Physics::mpDynamicsWorld->stepSimulation( timeStep, subSteps, fixedTimeStep );
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
        CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTexture( 0, pDiffuseTexture ));
        // each group has same texture
        mTextureChangeCount++;
        for( auto pMesh : meshes ) {
            // prevent overhead with normal texture
            if( pMesh->GetNormalTexture() ) {
                IDirect3DTexture9 * meshNormalTexture = pMesh->GetNormalTexture()->GetInterface();
                if( meshNormalTexture != pNormalTexture ) {
                    mTextureChangeCount++;
                    pMesh->GetNormalTexture()->Bind( 1 );
                    pNormalTexture = meshNormalTexture;
                }
            }
            if( !pMesh->mIndexBuffer || !pMesh->mVertexBuffer ) {
                continue;
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
            ruFreeRenderer();
        }
    }
}

LRESULT CALLBACK Engine::WindowProcess( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
    switch ( msg ) {
    case WM_DESTROY:
        ruFreeRenderer();
        PostQuitMessage ( 0 );
        break;

    case WM_ERASEBKGND:
        return 0;
    }

    return DefWindowProc ( wnd, msg, wParam, lParam );
}

void Engine::SetPixelShaderInt( UINT startRegister, int v ) {
    int buffer[ 4 ] = { v, 0, 0, 0 };
    Engine::Instance().GetDevice()->SetPixelShaderConstantI( startRegister, buffer, 1 );
}

void Engine::SetPixelShaderFloat( UINT startRegister, float v ) {
    float buffer[ 4 ] = { v, 0.0f, 0.0f, 0.0f };
    Engine::Instance().GetDevice()->SetPixelShaderConstantF( startRegister, buffer, 1 );
}

void Engine::SetPixelShaderFloat3( UINT startRegister, float * v ) {
    float buffer[ 4 ] = { v[0], v[1], v[2], 0.0f };
    Engine::Instance().GetDevice()->SetPixelShaderConstantF( startRegister, buffer, 1 );
}

void Engine::SetPixelShaderMatrix( UINT startRegister, D3DMATRIX * matrix ) {
    Engine::Instance().GetDevice()->SetPixelShaderConstantF( startRegister, &matrix->m[0][0], 4 );
}

void Engine::SetVertexShaderInt( UINT startRegister, int v ) {
    int buffer[ 4 ] = { v, 0, 0, 0 };
    Engine::Instance().GetDevice()->SetVertexShaderConstantI( startRegister, buffer, 1 );
}

void Engine::SetVertexShaderFloat( UINT startRegister, float v ) {
    float buffer[ 4 ] = { v, 0.0f, 0.0f, 0.0f };
    Engine::Instance().GetDevice()->SetVertexShaderConstantF( startRegister, buffer, 1 );
}

void Engine::SetVertexShaderFloat3( UINT startRegister, float * v ) {
    float buffer[ 4 ] = { v[0], v[1], v[2], 0.0f };
    Engine::Instance().GetDevice()->SetVertexShaderConstantF( startRegister, buffer, 1 );
}

void Engine::SetVertexShaderMatrix( UINT startRegister, D3DMATRIX * matrix ) {
    Engine::Instance().GetDevice()->SetVertexShaderConstantF( startRegister, &matrix->m[0][0], 4 );
}

void Engine::OnDeviceLost() {

}

Engine::Engine() {
	mpDevice = nullptr;
	mpDirect3D = nullptr;
	mpDeferredRenderer = nullptr;
	mpForwardRenderer = nullptr;
	mpParticleSystemRenderer = nullptr;
	mpTextRenderer = nullptr;
	mpGUIRenderer = nullptr;
	mAmbientColor = ruVector3( 0.05, 0.05, 0.05 );
	mUsePointLightShadows = false;
	mUseSpotLightShadows = false;
	mHDREnabled = false;
	mRunning = true;
	mFXAAEnabled = false;
	mTextureStoragePath = "data/textures/generic/";
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

void Engine::SetDiffuseNormalSamplersFiltration( D3DTEXTUREFILTERTYPE filter, bool disableMips )
{
	if( filter == D3DTEXF_NONE ) { // invalid argument to min and mag filters
		GetDevice()->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		GetDevice()->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		GetDevice()->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		GetDevice()->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	} else if( filter == D3DTEXF_LINEAR ) {
		GetDevice()->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		GetDevice()->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		GetDevice()->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		GetDevice()->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	} else if( filter == D3DTEXF_ANISOTROPIC ) {
		GetDevice()->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
		GetDevice()->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
		GetDevice()->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
		GetDevice()->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
	}

	// mip filters
	if( filter == D3DTEXF_NONE || disableMips ) { // actually disables mip-mapping
		GetDevice()->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		GetDevice()->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	} else if( filter == D3DTEXF_POINT ) {
		GetDevice()->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
		GetDevice()->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	} else if( filter == D3DTEXF_LINEAR ) {
		GetDevice()->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		GetDevice()->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	}
}

void Engine::SetAnisotropicTextureFiltration( bool state )
{
	mAnisotropicFiltering = state;
}

bool Engine::IsAnisotropicFilteringEnabled()
{
	return mAnisotropicFiltering;
}


//////////////////////////////////////////////////////////
// API
//////////////////////////////////////////////////////////

void ruSetSpotLightShadowMapSize( int size ) {
    Engine::Instance().SetSpotLightShadowMapSize( size );
}

void ruEnableSpotLightShadows( bool state ) {
    Engine::Instance().SetSpotLightShadowsEnabled( state );
}

bool ruIsSpotLightShadowsEnabled() {
    return Engine::Instance().IsSpotLightShadowsEnabled();
}

int ruDIPs( ) {
    return Engine::Instance().GetDIPCount();
}

int ruCreateRenderer( int width, int height, int fullscreen, char vSync ) {
    Engine::Instance().Initialize( width, height, fullscreen, vSync ) ;
    return 1;
}

void ruSetAmbientColor( ruVector3 color ) {
    Engine::Instance().SetAmbientColor( color );
}

int ruGetAvailableTextureMemory() {
    return Engine::Instance().GetDevice()->GetAvailableTextureMem();
}

ruNodeHandle ruCastRay( ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint ) {
    btVector3 rayEnd = btVector3 ( end.x, end.y, end.z );
    btVector3 rayBegin = btVector3 ( begin.x, begin.y, begin.z );

    btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
    Physics::mpDynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

    if ( rayCallback.hasHit() ) {
        const btRigidBody * pBody = btRigidBody::upcast ( rayCallback.m_collisionObject );
        if ( pBody ) {
            SceneNode * node = ( SceneNode * ) pBody->getUserPointer();

            if ( node ) {
                if( outPickPoint ) {
                    outPickPoint->x = rayCallback.m_hitPointWorld.x();
                    outPickPoint->y = rayCallback.m_hitPointWorld.y();
                    outPickPoint->z = rayCallback.m_hitPointWorld.z();
                };

                return SceneNode::HandleFromPointer( node );
            }
        }
    }

    return SceneNode::HandleFromPointer( 0 );
}

ruNodeHandle ruRayPick( int x, int y, ruVector3 * outPickPoint ) {
    D3DVIEWPORT9 vp;
    Engine::Instance().GetDevice()->GetViewport( &vp );
    // Find screen coordinates normalized to -1,1
    D3DXVECTOR3 coord;
    coord.x = ( ( ( 2.0f * x ) / (float)vp.Width ) - 1 );
    coord.y = - ( ( ( 2.0f * y ) / (float)vp.Height ) - 1 );
    coord.z = -1.0f;

    // Back project the ray from screen to the far clip plane
    coord.x /= Camera::msCurrentCamera->mProjection._11;
    coord.y /= Camera::msCurrentCamera->mProjection._22;

    D3DXMATRIX matinv = Camera::msCurrentCamera->mView;
    D3DXMatrixInverse( &matinv, NULL, &matinv );

    coord *= Camera::msCurrentCamera->mFarZ;
    D3DXVec3TransformCoord ( &coord, &coord, &matinv );

    btVector3 rayEnd = btVector3 ( coord.x, coord.y, coord.z );
    btVector3 rayBegin = Camera::msCurrentCamera->mGlobalTransform.getOrigin();

    btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
    Physics::mpDynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

    if ( rayCallback.hasHit() ) {
        const btRigidBody * pBody = btRigidBody::upcast ( rayCallback.m_collisionObject );
        if ( pBody ) {
            SceneNode * node = ( SceneNode * ) pBody->getUserPointer();

            if ( node ) {
                if( outPickPoint ) {
                    outPickPoint->x = rayCallback.m_hitPointWorld.x();
                    outPickPoint->y = rayCallback.m_hitPointWorld.y();
                    outPickPoint->z = rayCallback.m_hitPointWorld.z();
                };

                return SceneNode::HandleFromPointer( node );
            }
        }
    }

    return SceneNode::HandleFromPointer( 0 );
}

int ruGetRendererMaxAnisotropy() {
    D3DCAPS9 caps;
    CheckDXErrorFatal( Engine::Instance().GetDevice()->GetDeviceCaps( &caps ));

    return caps.MaxAnisotropy;
}

int ruFreeRenderer( ) {
    Engine::Instance().Shutdown();
    return 1;
}

int ruGetResolutionWidth( ) {
    return Engine::Instance().GetResolutionWidth();
}

int ruGetResolutionHeight( ) {
    return Engine::Instance().GetResolutionHeight();
}

int ruTextureUsedPerFrame( ) {
    return Engine::Instance().GetTextureChangeCount();
}

int ruRenderWorld( ) {
    Engine::Instance().RenderWorld();
    return 1;
}

void ruEnableShadows( bool state ) {
    Engine::Instance().SetSpotLightShadowsEnabled( state );
    Engine::Instance().SetPointLightShadowsEnabled( state );
}

void ruSetHDREnabled( bool state ) {
    Engine::Instance().SetHDREnabled( state );
}

void ruSetAnisotropicTextureFiltration( bool state ) {
	Engine::Instance().SetAnisotropicTextureFiltration( state );
}

bool ruIsHDREnabled( ) {
    return Engine::Instance().IsHDREnabled();
}

void ruUpdateWorld() {
	for( auto node : SceneNode::msNodeList ) {
		node->CalculateGlobalTransform();
	}
}