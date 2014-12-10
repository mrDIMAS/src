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

Renderer * g_renderer = 0;

IDirect3D9 * g_d3d = 0;
IDirect3DDevice9 * g_device = 0;
IDirect3DVertexDeclaration9 * g_meshVertexDeclaration = 0;



float g_width = 0;
float g_height = 0;
int g_dips = 0;
int g_debugDraw = 0;
int g_textureChanges = 0;
int g_fps = 0;
float g_dt = 1.0f / 60.0f; // 60 FPS

FPSCounter g_fpsCounter;
bool g_usePointLightShadows = true;
bool g_useSpotLightShadows = true;
bool g_engineRunning = true;
bool g_hdrEnabled = false;

ruVector3 g_ambientColor = ruVector3( 0.05, 0.05, 0.05 );

vector< Light*> affectedLights;

Renderer::~Renderer()
{
    for( auto fnt : BitmapFont::fonts )
        delete fnt;
    for( auto tmr : Timer::timers )
        delete tmr;
    for( auto & kv : CubeTexture::all )
        delete kv.second;
    FT_Done_FreeType( g_ftLibrary );
    delete g_textRenderer;
    delete g_particleSystemRenderer;
    delete g_deferredRenderer;
    delete g_guiRenderer;
    while( g_nodes.size() )
        delete g_nodes.front();
    if( g_forwardRenderer )
        delete g_forwardRenderer;
    Texture::DeleteAll();
    g_meshVertexDeclaration->Release();
    int counter = 0;
    if( g_device )
    {
        while( g_device->Release() )
            counter++;
    }
    if( g_d3d )
    {
        while( g_d3d->Release() )
            counter++;
    }
    Physics::DestructWorld();
    pfSystemDestroy();
    CloseLogFile();
}

Renderer::Renderer( int width, int height, int fullscreen, char vSync )
{
    if ( width == 0 )
        width = GetSystemMetrics ( SM_CXSCREEN );
    if ( height == 0 )
        height = GetSystemMetrics ( SM_CYSCREEN );
    if( !CreateRenderWindow( width, height, fullscreen ))
        return;

    CreateLogFile();

    // try to create Direct3D9
    g_d3d = Direct3DCreate9( D3D_SDK_VERSION );

    // epic fail
    if( !g_d3d )
    {
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
    if( psVerHi < 3 )
    {
        MessageBoxA( 0, "Your graphics card doesn't support Pixel Shader 3.0. Engine initialization failed! Buy a modern video card!", "Epic fail", 0 );
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
    if( vSync )
        presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    else
        presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
    presentParameters.BackBufferWidth = width;
    presentParameters.BackBufferHeight = height;

    if ( fullscreen )
    {
        presentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
        presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        presentParameters.Windowed = FALSE;
        presentParameters.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
    }
    else
    {
        presentParameters.BackBufferFormat = displayMode.Format;
        presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        presentParameters.Windowed = TRUE;
    }

    // no multisampling, because of deferred shading
    presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
    presentParameters.MultiSampleQuality = 0;

    // create device
    CheckDXErrorFatal( g_d3d->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE, &presentParameters, &g_device ));

    // create main "pipeline" vertex declaration
    D3DVERTEXELEMENT9 vd[ ] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        D3DDECL_END()
    };

    CheckDXErrorFatal( g_device->CreateVertexDeclaration( vd, &g_meshVertexDeclaration ));

    CheckDXErrorFatal( g_device->SetRenderState ( D3DRS_LIGHTING, FALSE ));
    CheckDXErrorFatal( g_device->SetRenderState ( D3DRS_ZENABLE, TRUE ));
    CheckDXErrorFatal( g_device->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE ));
    CheckDXErrorFatal( g_device->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL ));
    CheckDXErrorFatal( g_device->SetRenderState ( D3DRS_ALPHAREF, 100 ));
    CheckDXErrorFatal( g_device->SetRenderState ( D3DRS_ALPHATESTENABLE, TRUE ));
    CheckDXErrorFatal( g_device->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL ));
    CheckDXErrorFatal( g_device->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW ));

    CheckDXErrorFatal( g_device->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 1, D3DSAMP_MAXANISOTROPY, dCaps.MaxAnisotropy ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR ));

    CreatePhysics( );
    pfSystemInit( );
    pfSetListenerDopplerFactor( 0 );

    g_deferredRenderer = new MultipleRTDeferredRenderer();
    g_forwardRenderer = new ForwardRenderer();
    g_particleSystemRenderer = new ParticleSystemRenderer();
    g_textRenderer = new TextRenderer();
    g_guiRenderer = new GUIRenderer();
    g_renderer = this;
    performanceTimer = new Timer;

    // init freetype
    if( FT_Init_FreeType( &g_ftLibrary ) )
        throw std::runtime_error( "Unable to initialize FreeType 2.53" );
}

/*
==========
Renderer::IsMeshVisible
==========
*/
bool Renderer::IsMeshVisible( Mesh * mesh )
{
    mesh->ownerNode->inFrustum = g_camera->frustum.IsAABBInside( mesh->aabb, ruVector3( mesh->ownerNode->globalTransform.getOrigin().m_floats ));
    return mesh->ownerNode->skinned || mesh->ownerNode->IsVisible() && mesh->ownerNode->inFrustum;
}

/*
==========
Renderer::CreateRenderWindow
==========
*/
int Renderer::CreateRenderWindow( int width, int height, int fullscreen )
{
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
    if ( !fullscreen )
    {
        // windowed style prevent device lost
        style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    }
    else
    {
        // in fullscreen we has to deal with device lost
        style = WS_POPUP;
    }
    RECT wRect = { 0, 0, width, height };
    // make client region fits to the current resolution
    AdjustWindowRect ( &wRect, style, 0 );
    // create window
    window = CreateWindowW( className, className, style, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, instance, 0 );

    if( !window )   // fail
        return 0;
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
void Renderer::CreatePhysics()
{
    Physics::CreateWorld();
}

/*
===============
Renderer::RenderWorld
===============
*/
void Renderer::RenderWorld()
{
    if( !g_engineRunning )
        return;
    if( !g_camera )
        return;
    g_fpsCounter.RegisterFrame();
    // erase marked nodes
    SceneNode::EraseUnusedNodes();
    // window message pump
    UpdateMessagePump();
    // clear statistics
    g_dips = 0;
    g_textureChanges = 0;
    // build view and projection matrices, frustum, also attach sound listener to camera
    g_camera->Update();
    // precalculations
    for( auto node : g_nodes )
    {
        node->CalculateGlobalTransform();
        node->PerformAnimation();
        // update all sounds attached to node, and physical interaction sounds( roll, hit )
        node->UpdateSounds();
        // skip frustum flag, it will be set to true, if one of node's mesh
        // are in frustum
        node->inFrustum = false;
    }
    // update lights
    for( auto light : g_spotLights )
        light->DoFloating();
    for( auto light : g_pointLights )
        light->DoFloating();
    // begin dx scene
    CheckDXErrorFatal( g_device->BeginScene());
    // begin rendering into G-Buffer
    g_deferredRenderer->BeginFirstPass();
    // setup samplers
    CheckDXErrorFatal( g_device->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC ));

    // render from current camera
    RenderMeshesIntoGBuffer();
    // set samplers to linear filtering
    CheckDXErrorFatal( g_device->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ));
    // end render into G-Buffer and do a lighting passes
    g_deferredRenderer->EndFirstPassAndDoSecondPass();
    // render all opacity meshes with forward renderer
    g_forwardRenderer->RenderMeshes();
    // render particles after all, cause deferred shading doesnt support transparency
    g_particleSystemRenderer->RenderAllParticleSystems();
    // render gui on top of all
    g_guiRenderer->RenderAllGUIElements();
    // render light flares without writing to z-buffer
    Light::RenderLightFlares();
    // finalize
    CheckDXErrorFatal( g_device->EndScene());
    CheckDXErrorFatal( g_device->Present( 0, 0, 0, 0 ));
    // grab info about node's physic contacts
    SceneNode::UpdateContacts( );
    // update sound subsystem
    pfSystemUpdate();
    // update physics subsystem
    /*
    int subSteps = g_dt / ( 1.0f / 60.0f );
    if( subSteps < 1 )
    	subSteps = 1;
    if( g_physicsEnabled ) {
    	for( int i = 0; i < subSteps; i++ )
    		g_dynamicsWorld->stepSimulation( 1.0f / 60.0f, 1 );
    }*/
    if( g_physicsEnabled )
    {
        float dt = g_dt;
        if( dt < (1.0f / 60.0f) )
            dt = (1.0f / 60.0f) ;
        g_dynamicsWorld->stepSimulation( dt, 60 );
    }
}

/*
===============
Renderer::RenderMeshesIntoGBuffer

all registered meshes are sorted by texture, so rendering becomes really fast - there no redundant texture changes
===============
*/
void Renderer::RenderMeshesIntoGBuffer()
{
    for( auto groupIterator : Mesh::meshes )
    {
        IDirect3DTexture9 * diffuseTexture = groupIterator.first;
        IDirect3DTexture9 * normalTexture = nullptr;
        auto & meshes = groupIterator.second;
        // skip group if it has no meshes
        if( meshes.size() == 0 )
            continue;
        // bind diffuse texture
        CheckDXErrorFatal( g_device->SetTexture( 0, diffuseTexture ));
        // each group has same texture
        g_textureChanges++;
        for( auto meshIterator : meshes )
        {
            Mesh * mesh = meshIterator;
            SceneNode * node = mesh->ownerNode;
            // prevent overhead with normal texture
            if( mesh->GetNormalTexture() )
            {
                IDirect3DTexture9 * meshNormalTexture = mesh->GetNormalTexture()->GetInterface();
                if( meshNormalTexture != normalTexture )
                {
                    mesh->GetNormalTexture()->Bind( 1 );
                    normalTexture = meshNormalTexture;
                }
            }
            if( IsMeshVisible( mesh ) )
            {
                if( !mesh->indexBuffer || !mesh->vertexBuffer )
                    continue;
                g_deferredRenderer->RenderMesh( mesh );
            }
        }
    }
}
/*
===============
Renderer::UpdateMessagePump
===============
*/
void Renderer::UpdateMessagePump()
{
    MSG message;

    while ( PeekMessage ( &message, NULL, 0, 0, PM_REMOVE ) )
    {
        DispatchMessage ( &message );

        if ( message.message == WM_QUIT )
            ruFreeRenderer();
    }
}
/*
===============
Renderer::WindowProcess
===============
*/
LRESULT CALLBACK Renderer::WindowProcess( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch ( msg )
    {
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


//////////////////////////////////////////////////////////
// API
//////////////////////////////////////////////////////////

/*
===============
SetPointLightShadowMapSize
===============
*/
RUAPI void ruSetPointLightShadowMapSize( int size )
{
    g_deferredRenderer->SetPointLightShadowMapSize( size );
}

/*
===============
SetSpotLightShadowMapSize
===============
*/
RUAPI void ruSetSpotLightShadowMapSize( int size )
{
    g_deferredRenderer->SetSpotLightShadowMapSize( size );
}

/*
===============
EnablePointLightShadows
===============
*/
RUAPI void ruEnablePointLightShadows( bool state )
{
    g_usePointLightShadows = state;
}

/*
===============
EnableSpotLightShadows
===============
*/
RUAPI void ruEnableSpotLightShadows( bool state )
{
    g_useSpotLightShadows = state;
}

/*
===============
EnableSpotLightShadows
===============
*/
RUAPI bool ruIsPointLightShadowsEnabled()
{
    return g_usePointLightShadows;
}

/*
===============
EnableSpotLightShadows
===============
*/
RUAPI bool ruIsSpotLightShadowsEnabled()
{
    return g_useSpotLightShadows;
}

/*
===============
DebugDrawEnabled
===============
*/
void ruDebugDrawEnabled( int state )
{
    g_debugDraw = state;
}

/*
===============
SetTextureFiltering
===============
*/
void ruSetRendererTextureFiltering( const int & filter, int anisotropicQuality )
{
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
    CheckDXErrorFatal( g_device->SetSamplerState ( 0, D3DSAMP_MINFILTER, minMagFilter ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 0, D3DSAMP_MIPFILTER, mipFilter ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 0, D3DSAMP_MAGFILTER, minMagFilter ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, anisotropicQuality ));

    CheckDXErrorFatal( g_device->SetSamplerState ( 1, D3DSAMP_MINFILTER, minMagFilter ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 1, D3DSAMP_MIPFILTER, mipFilter ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 1, D3DSAMP_MAGFILTER, minMagFilter ));
    CheckDXErrorFatal( g_device->SetSamplerState ( 1, D3DSAMP_MAXANISOTROPY, anisotropicQuality ));
}
/*
===============
DIPs
===============
*/
int ruDIPs( )
{
    return g_dips;
}
/*
===============
CreateRenderer
===============
*/
int ruCreateRenderer( int width, int height, int fullscreen, char vSync )
{
    Renderer * renderer = new Renderer( width, height, fullscreen, vSync ) ;
    return 1;
}

/*
===============
SetAmbientColor
===============
*/
void ruSetAmbientColor( ruVector3 color )
{
    g_ambientColor = color;
}

/*
===============
GetAvailableTextureMemory
===============
*/
int ruGetAvailableTextureMemory()
{
    return g_device->GetAvailableTextureMem();
}

/*
===============
RayTest
===============
*/
RUAPI ruNodeHandle ruCastRay( ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint )
{
    btVector3 rayEnd = btVector3 ( end.x, end.y, end.z );
    btVector3 rayBegin = btVector3 ( begin.x, begin.y, begin.z );

    btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
    g_dynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

    if ( rayCallback.hasHit() )
    {
        const btRigidBody * pBody = btRigidBody::upcast ( rayCallback.m_collisionObject );
        if ( pBody )
        {
            SceneNode * node = ( SceneNode * ) pBody->getUserPointer();

            if ( node )
            {
                if( outPickPoint )
                {
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
ruNodeHandle ruRayPick( int x, int y, ruVector3 * outPickPoint )
{
    D3DVIEWPORT9 vp;
    g_device->GetViewport( &vp );
    // Find screen coordinates normalized to -1,1
    D3DXVECTOR3 coord;
    coord.x = ( ( ( 2.0f * x ) / (float)vp.Width ) - 1 );
    coord.y = - ( ( ( 2.0f * y ) / (float)vp.Height ) - 1 );
    coord.z = -1.0f;

    // Back project the ray from screen to the far clip plane
    coord.x /= g_camera->projection._11;
    coord.y /= g_camera->projection._22;

    D3DXMATRIX matinv = g_camera->view;
    D3DXMatrixInverse( &matinv, NULL, &matinv );

    coord *= g_camera->farZ;
    D3DXVec3TransformCoord ( &coord, &coord, &matinv );

    btVector3 rayEnd = btVector3 ( coord.x, coord.y, coord.z );
    btVector3 rayBegin = g_camera->globalTransform.getOrigin();

    btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
    g_dynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

    if ( rayCallback.hasHit() )
    {
        const btRigidBody * pBody = btRigidBody::upcast ( rayCallback.m_collisionObject );
        if ( pBody )
        {
            SceneNode * node = ( SceneNode * ) pBody->getUserPointer();

            if ( node )
            {
                if( outPickPoint )
                {
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
int ruGetRendererMaxAnisotropy()
{
    D3DCAPS9 caps;
    CheckDXErrorFatal( g_device->GetDeviceCaps( &caps ));

    return caps.MaxAnisotropy;
}

/*
===============
FreeRenderer
===============
*/
int ruFreeRenderer( )
{
    g_engineRunning = false;
    delete g_renderer;
    return 1;
}

/*
===============
GetResolutionWidth
===============
*/
int ruGetResolutionWidth( )
{
    return g_width;
}

/*
===============
GetResolutionHeight
===============
*/
int ruGetResolutionHeight( )
{
    return g_height;
}

/*
===============
TextureUsedPerFrame
===============
*/
int ruTextureUsedPerFrame( )
{
    return g_textureChanges;
}

/*
===============
SetRenderQuality
===============
*/
void ruSetRenderQuality( char renderQuality )
{
    g_deferredRenderer->SetRenderingQuality( renderQuality );
}

/*
===============
RenderWorld
===============
*/
int ruRenderWorld( float dt )
{
    g_dt = dt;
    g_renderer->RenderWorld();
    return 1;
}

/*
===============
EnableShadows
===============
*/
RUAPI void ruEnableShadows( bool state )
{
    g_useSpotLightShadows = state;
    g_usePointLightShadows = state;
}

/*
===============
SetHDREnabled
===============
*/
RUAPI void ruSetHDREnabled( bool state )
{
    g_hdrEnabled = state;
}

/*
===============
IsHDREnabled
===============
*/
RUAPI bool ruIsHDREnabled( )
{
    return g_hdrEnabled;
}

/*
===============
SetHDRExposure
===============
*/
RUAPI void ruSetHDRExposure( float exposure )
{
    //g_deferredRenderer->hdrRenderer->exposure = exposure;
}

/*
===============
GetHDRExposure
===============
*/
RUAPI float ruGetHDRExposure( )
{
    return 1.0f;//g_deferredRenderer->hdrRenderer->exposure;
}