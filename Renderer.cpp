#include "Precompiled.h"

#include "Common.h"
#include "ParticleSystemRenderer.h"
#include "GUIRenderer.h"
#include "Light.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Renderer.h"
#include "Physics.h"
#include "MultipleRTDeferredRenderer.h"
#include "FXAA.h"
#include "FPSCounter.h"
#include "ForwardRenderer.h"
#include "BitmapFont.h"
#include "TextRenderer.h"

Renderer * gpRenderer = 0;

IDirect3D9 * g_d3d = 0;
IDirect3DDevice9 * gpDevice = 0;
IDirect3DVertexDeclaration9 * g_meshVertexDeclaration = 0;

float g_width = 0;
float g_height = 0;
int g_dips = 0;
int g_debugDraw = 0;
int g_textureChanges = 0;
int g_fps = 0;

FPSCounter g_fpsCounter;
bool g_usePointLightShadows = true;
bool g_useSpotLightShadows = true;
bool g_engineRunning = true;
bool g_hdrEnabled = false;

ruVector3 g_ambientColor = ruVector3( 0.05, 0.05, 0.05 );

vector< Light*> affectedLights;

Renderer::~Renderer() {
	
    for( auto fnt : BitmapFont::fonts ) {
        delete fnt;
    }
    for( auto tmr : Timer::timers ) {
        delete tmr;
    }
    for( auto & kv : CubeTexture::all ) {
        delete kv.second;
    }
    FT_Done_FreeType( g_ftLibrary );
    delete g_textRenderer;
    delete g_particleSystemRenderer;
    delete g_deferredRenderer;
    delete g_guiRenderer;
    while( GUINode::msNodeList.size() ) {
        delete GUINode::msNodeList.front();
    }
    while( g_nodes.size() ) {
        delete g_nodes.front();
    }
	Mesh::EraseAll();
    if( g_forwardRenderer ) {
        delete g_forwardRenderer;
    }
    Texture::DeleteAll();
    g_meshVertexDeclaration->Release();
    int counter = 0;
    if( gpDevice ) {
        while( gpDevice->Release() ) {
            counter++;
        }
    }
    if( g_d3d ) {
        while( g_d3d->Release() ) {
            counter++;
        }
    }
    Physics::DestructWorld();
    pfSystemDestroy();
    CloseLogFile();
}

Renderer::Renderer( int width, int height, int fullscreen, char vSync ) {
    if ( width == 0 ) {
        width = GetSystemMetrics ( SM_CXSCREEN );
    }
    if ( height == 0 ) {
        height = GetSystemMetrics ( SM_CYSCREEN );
    }
    if( !CreateRenderWindow( width, height, fullscreen )) {
        return;
    }

    CreateLogFile();

    // try to create Direct3D9
    g_d3d = Direct3DCreate9( D3D_SDK_VERSION );

    // epic fail
    if( !g_d3d ) {
        MessageBoxA( 0, "Failed to Direct3DCreate9! Ensure, that you have latest video drivers! Engine initialization failed!", "ERROR", MB_OK | MB_ICONERROR );
        CloseLogFile();
        exit( -1 );
    }

    // check d3d caps, to ensure, that user have modern hardware
    D3DCAPS9 dCaps;
    CheckDXErrorFatal( g_d3d->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &dCaps ));

    unsigned char psVerHi = D3DSHADER_VERSION_MAJOR( dCaps.PixelShaderVersion );
    unsigned char psVerLo = D3DSHADER_VERSION_MINOR( dCaps.PixelShaderVersion );

    // epic fail
    if( psVerHi < 2 ) {
        MessageBoxA( 0, "Your graphics card doesn't support Pixel Shader 2.0. Engine initialization failed! Buy a modern video card!", "Epic fail", 0 );
        CloseLogFile();
        g_d3d->Release();
        exit( -1 );
    }

    g_width = width;
    g_height = height;

    D3DDISPLAYMODE displayMode = { 0 };
    g_d3d->GetAdapterDisplayMode ( D3DADAPTER_DEFAULT, &displayMode );

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
    if( FAILED( g_d3d->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters, &gpDevice )))
	{
		MessageBoxA( 0, "Engine initialization failed! Buy a modern video card!", "Epic fail", 0 );
		CloseLogFile();
		g_d3d->Release();
		exit( -1 );
	}

    // create main "pipeline" vertex declaration
    D3DVERTEXELEMENT9 vd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        D3DDECL_END()
    };

    CheckDXErrorFatal( gpDevice->CreateVertexDeclaration( vd, &g_meshVertexDeclaration ));

    gpDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
    gpDevice->SetRenderState ( D3DRS_ZENABLE, TRUE );
    gpDevice->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
    gpDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
    gpDevice->SetRenderState ( D3DRS_ALPHAREF, 10 );
    gpDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    gpDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
    gpDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );

    gpDevice->SetRenderState( D3DRS_STENCILREF, 0x0 );
    gpDevice->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
    gpDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
    gpDevice->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE );
    gpDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
    gpDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR );
    gpDevice->SetRenderState( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_INCR );

    // setup samplers
    gpDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    gpDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    gpDevice->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    gpDevice->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    gpDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    gpDevice->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    gpDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    gpDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
    CreatePhysics( );
    pfSystemInit( );
    pfSetListenerDopplerFactor( 0 );

    g_deferredRenderer = new MultipleRTDeferredRenderer();
    g_forwardRenderer = new ForwardRenderer();
    g_particleSystemRenderer = new ParticleSystemRenderer();
    g_textRenderer = new TextRenderer();
    g_guiRenderer = new GUIRenderer();
    gpRenderer = this;
    performanceTimer = new Timer;

    // init freetype
    if( FT_Init_FreeType( &g_ftLibrary ) ) {
        throw std::runtime_error( "Unable to initialize FreeType 2.53" );
    }
}

bool IsFullNPOTTexturesSupport()
{
	D3DCAPS9 caps;
	gpDevice->GetDeviceCaps( &caps );
	char npotcond = caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL;
	char pot = caps.TextureCaps & D3DPTEXTURECAPS_POW2;
	return !(npotcond || pot);
}

/*
==========
Renderer::CreateRenderWindow
==========
*/
int Renderer::CreateRenderWindow( int width, int height, int fullscreen ) {
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

/*
===============
Renderer::CreatePhysics
===============
*/
void Renderer::CreatePhysics() {
    Physics::CreateWorld();
}

/*
===============
Renderer::RenderWorld
===============
*/
void Renderer::RenderWorld() {
    if( !g_engineRunning ) {
        return;
    }
    if( !g_camera ) {
        return;
    }
    g_fpsCounter.RegisterFrame();
    // erase marked nodes
	Mesh::EraseOrphanMeshes();
    SceneNode::EraseUnusedNodes();
    // window message pump
    UpdateMessagePump();
    // clear statistics
    g_dips = 0;
    g_textureChanges = 0;
    // build view and projection matrices, frustum, also attach sound listener to camera
    g_camera->Update();
    // precalculations
    for( auto node : g_nodes ) {
        node->CalculateGlobalTransform();
        node->PerformAnimation();
        // update all sounds attached to node, and physical interaction sounds( roll, hit )
        node->UpdateSounds();
        // skip frustum flag, it will be set to true, if one of node's mesh
        // are in frustum
        node->mInFrustum = false;
    }
    // update lights
    for( auto light : g_spotLightList ) {
        light->DoFloating();
    }
    for( auto light : g_pointLightList ) {
        light->DoFloating();
    }
    // begin dx scene
    CheckDXErrorFatal( gpDevice->BeginScene());
    // begin rendering into G-Buffer
    gpDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    gpDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    gpDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    gpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_deferredRenderer->BeginFirstPass();
    // render from current camera
    RenderMeshesIntoGBuffer();
    // end render into G-Buffer and do a lighting passes
    if( g_hdrEnabled ) {
        gpDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, TRUE );
        gpDevice->SetSamplerState( 2, D3DSAMP_SRGBTEXTURE, TRUE );
    } else {
        gpDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
        gpDevice->SetSamplerState( 2, D3DSAMP_SRGBTEXTURE, FALSE );
    }
	gpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	gpDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    gpDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    gpDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    gpDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    gpDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    gpDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
    g_deferredRenderer->EndFirstPassAndDoSecondPass();
    // render all opacity meshes with forward renderer
    gpDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	gpDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    gpDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	gpDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	gpDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_forwardRenderer->RenderMeshes();
	gpDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    // render particles after all, because deferred shading doesnt support transparency
    gpDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    gpDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );

    g_particleSystemRenderer->RenderAllParticleSystems();
    // render gui on top of all
    gpDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    g_guiRenderer->RenderAllGUIElements();
    // render light flares without writing to z-buffer
    //Light::RenderLightFlares();
    // finalize
    gpDevice->EndScene();
    if( gpDevice->Present( 0, 0, 0, 0 ) == D3DERR_DEVICELOST ) {
		g_deferredRenderer->OnDeviceLost();
		OnDeviceLost();
	}
    // grab info about node's physic contacts
    SceneNode::UpdateContacts( );
    // update sound subsystem
    pfSystemUpdate();
}

RUAPI void ruUpdatePhysics( float timeStep, int subSteps, float fixedTimeStep ) {
    g_dynamicsWorld->stepSimulation( timeStep, subSteps, fixedTimeStep );
}
/*
===============
Renderer::RenderMeshesIntoGBuffer

all registered meshes are sorted by texture, so rendering becomes really fast - there no redundant texture changes
===============
*/
void Renderer::RenderMeshesIntoGBuffer() {
    for( auto groupIterator : Mesh::msMeshList ) {
        IDirect3DTexture9 * pDiffuseTexture = groupIterator.first;
        IDirect3DTexture9 * pNormalTexture = nullptr;
        auto & meshes = groupIterator.second;
        // skip group if it has no meshes
        if( meshes.size() == 0 ) {
            continue;
        }
        // bind diffuse texture
        CheckDXErrorFatal( gpDevice->SetTexture( 0, pDiffuseTexture ));
        // each group has same texture
        g_textureChanges++;
        for( auto pMesh : meshes ) {
            // prevent overhead with normal texture
            if( pMesh->GetNormalTexture() ) {
                IDirect3DTexture9 * meshNormalTexture = pMesh->GetNormalTexture()->GetInterface();
                if( meshNormalTexture != pNormalTexture ) {
                    g_textureChanges++;
                    pMesh->GetNormalTexture()->Bind( 1 );
                    pNormalTexture = meshNormalTexture;
                }
            }
            if( !pMesh->mIndexBuffer || !pMesh->mVertexBuffer ) {
                continue;
            }
            g_deferredRenderer->RenderMesh( pMesh );            
        }
    }
}
/*
===============
Renderer::UpdateMessagePump
===============
*/
void Renderer::UpdateMessagePump() {
    MSG message;
    while ( PeekMessage ( &message, NULL, 0, 0, PM_REMOVE ) ) {
        DispatchMessage ( &message );
        if ( message.message == WM_QUIT ) {
            ruFreeRenderer();
        }
    }
}
/*
===============
Renderer::WindowProcess
===============
*/
LRESULT CALLBACK Renderer::WindowProcess( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
    switch ( msg ) {
    case WM_PAINT:
        PAINTSTRUCT ps;
        BeginPaint ( wnd, &ps );
        EndPaint ( wnd, &ps );
        return 0;

    case WM_DESTROY:
        ruFreeRenderer();
        PostQuitMessage ( 0 );
        break;

    case WM_ERASEBKGND:
        return 0;
    }

    return DefWindowProc ( wnd, msg, wParam, lParam );
}

void Renderer::SetPixelShaderInt( UINT startRegister, int v ) {
    int buffer[ 4 ] = { v, 0, 0, 0 };
    gpDevice->SetPixelShaderConstantI( startRegister, buffer, 1 );
}

void Renderer::SetPixelShaderFloat( UINT startRegister, float v ) {
    float buffer[ 4 ] = { v, 0.0f, 0.0f, 0.0f };
    gpDevice->SetPixelShaderConstantF( startRegister, buffer, 1 );
}

void Renderer::SetPixelShaderFloat3( UINT startRegister, float * v ) {
    float buffer[ 4 ] = { v[0], v[1], v[2], 0.0f };
    gpDevice->SetPixelShaderConstantF( startRegister, buffer, 1 );
}

void Renderer::SetPixelShaderMatrix( UINT startRegister, D3DMATRIX * matrix ) {
    gpDevice->SetPixelShaderConstantF( startRegister, &matrix->m[0][0], 4 );
}

void Renderer::SetVertexShaderInt( UINT startRegister, int v ) {
    int buffer[ 4 ] = { v, 0, 0, 0 };
    gpDevice->SetVertexShaderConstantI( startRegister, buffer, 1 );
}

void Renderer::SetVertexShaderFloat( UINT startRegister, float v ) {
    float buffer[ 4 ] = { v, 0.0f, 0.0f, 0.0f };
    gpDevice->SetVertexShaderConstantF( startRegister, buffer, 1 );
}

void Renderer::SetVertexShaderFloat3( UINT startRegister, float * v ) {
    float buffer[ 4 ] = { v[0], v[1], v[2], 0.0f };
    gpDevice->SetVertexShaderConstantF( startRegister, buffer, 1 );
}

void Renderer::SetVertexShaderMatrix( UINT startRegister, D3DMATRIX * matrix ) {
    gpDevice->SetVertexShaderConstantF( startRegister, &matrix->m[0][0], 4 );
}


//////////////////////////////////////////////////////////
// API
//////////////////////////////////////////////////////////

/*
===============
SetSpotLightShadowMapSize
===============
*/
RUAPI void ruSetSpotLightShadowMapSize( int size ) {
    g_deferredRenderer->SetSpotLightShadowMapSize( size );
}

/*
===============
EnableSpotLightShadows
===============
*/
RUAPI void ruEnableSpotLightShadows( bool state ) {
    g_useSpotLightShadows = state;
}

/*
===============
EnableSpotLightShadows
===============
*/
RUAPI bool ruIsSpotLightShadowsEnabled() {
    return g_useSpotLightShadows;
}

/*
===============
DebugDrawEnabled
===============
*/
void ruDebugDrawEnabled( int state ) {
    g_debugDraw = state;
}

/*
===============
SetTextureFiltering
===============
*/
void ruSetRendererTextureFiltering( const int & filter, int anisotropicQuality ) {
    /*
    int minMagFilter = D3DTEXF_POINT;

    if( filter == ruTextureFilter::Nearest )
        minMagFilter = D3DTEXF_POINT;
    if( filter == ruTextureFilter::Linear )
        minMagFilter = D3DTEXF_LINEAR;
    if( filter == ruTextureFilter::Anisotropic )
        minMagFilter = D3DTEXF_ANISOTROPIC;
    int mipFilter = D3DTEXF_LINEAR;

    if( mipFilter == ruTextureFilter::Nearest )
        mipFilter = D3DTEXF_POINT;
    CheckDXErrorFatal( g_pDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, minMagFilter ));
    CheckDXErrorFatal( g_pDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, mipFilter ));
    CheckDXErrorFatal( g_pDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, minMagFilter ));
    CheckDXErrorFatal( g_pDevice->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, anisotropicQuality ));

    CheckDXErrorFatal( g_pDevice->SetSamplerState ( 1, D3DSAMP_MINFILTER, minMagFilter ));
    CheckDXErrorFatal( g_pDevice->SetSamplerState ( 1, D3DSAMP_MIPFILTER, mipFilter ));
    CheckDXErrorFatal( g_pDevice->SetSamplerState ( 1, D3DSAMP_MAGFILTER, minMagFilter ));
    CheckDXErrorFatal( g_pDevice->SetSamplerState ( 1, D3DSAMP_MAXANISOTROPY, anisotropicQuality ));*/
}
/*
===============
DIPs
===============
*/
int ruDIPs( ) {
    return g_dips;
}
/*
===============
CreateRenderer
===============
*/
int ruCreateRenderer( int width, int height, int fullscreen, char vSync ) {
    Renderer * renderer = new Renderer( width, height, fullscreen, vSync ) ;
    return 1;
}

/*
===============
SetAmbientColor
===============
*/
void ruSetAmbientColor( ruVector3 color ) {
    g_ambientColor = color;
}

/*
===============
GetAvailableTextureMemory
===============
*/
int ruGetAvailableTextureMemory() {
    return gpDevice->GetAvailableTextureMem();
}

/*
===============
RayTest
===============
*/
RUAPI ruNodeHandle ruCastRay( ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint ) {
    btVector3 rayEnd = btVector3 ( end.x, end.y, end.z );
    btVector3 rayBegin = btVector3 ( begin.x, begin.y, begin.z );

    btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
    g_dynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

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

/*
===============
RayPick
===============
*/
ruNodeHandle ruRayPick( int x, int y, ruVector3 * outPickPoint ) {
    D3DVIEWPORT9 vp;
    gpDevice->GetViewport( &vp );
    // Find screen coordinates normalized to -1,1
    D3DXVECTOR3 coord;
    coord.x = ( ( ( 2.0f * x ) / (float)vp.Width ) - 1 );
    coord.y = - ( ( ( 2.0f * y ) / (float)vp.Height ) - 1 );
    coord.z = -1.0f;

    // Back project the ray from screen to the far clip plane
    coord.x /= g_camera->mProjection._11;
    coord.y /= g_camera->mProjection._22;

    D3DXMATRIX matinv = g_camera->mView;
    D3DXMatrixInverse( &matinv, NULL, &matinv );

    coord *= g_camera->mFarZ;
    D3DXVec3TransformCoord ( &coord, &coord, &matinv );

    btVector3 rayEnd = btVector3 ( coord.x, coord.y, coord.z );
    btVector3 rayBegin = g_camera->mGlobalTransform.getOrigin();

    btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
    g_dynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

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
/*
===============
GetMaxAnisotropy
===============
*/
int ruGetRendererMaxAnisotropy() {
    D3DCAPS9 caps;
    CheckDXErrorFatal( gpDevice->GetDeviceCaps( &caps ));

    return caps.MaxAnisotropy;
}

/*
===============
FreeRenderer
===============
*/
int ruFreeRenderer( ) {
    g_engineRunning = false;
    delete gpRenderer;
    return 1;
}

/*
===============
GetResolutionWidth
===============
*/
int ruGetResolutionWidth( ) {
    return g_width;
}

/*
===============
GetResolutionHeight
===============
*/
int ruGetResolutionHeight( ) {
    return g_height;
}

/*
===============
TextureUsedPerFrame
===============
*/
int ruTextureUsedPerFrame( ) {
    return g_textureChanges;
}

/*
===============
RenderWorld
===============
*/
int ruRenderWorld( ) {
    gpRenderer->RenderWorld();
    return 1;
}

/*
===============
EnableShadows
===============
*/
RUAPI void ruEnableShadows( bool state ) {
    g_useSpotLightShadows = state;
    g_usePointLightShadows = state;
}

/*
===============
SetHDREnabled
===============
*/
RUAPI void ruSetHDREnabled( bool state ) {
    g_hdrEnabled = state;
}

/*
===============
IsHDREnabled
===============
*/
RUAPI bool ruIsHDREnabled( ) {
    return g_hdrEnabled;
}

/*
===============
SetHDRExposure
===============
*/
void ruSetHDRExposure( float exposure ) {
    //g_deferredRenderer->hdrRenderer->exposure = exposure;
}

/*
===============
GetHDRExposure
===============
*/
float ruGetHDRExposure( ) {
    return 1.0f;//g_deferredRenderer->hdrRenderer->exposure;
}

/*
===============
GetHDRExposure
===============
*/
void ruUpdateWorld() {
	for( auto node : g_nodes ) {
		node->CalculateGlobalTransform();
	}
}