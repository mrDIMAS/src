#include "Common.h"
#include "ParticleSystemRenderer.h"
#include "GUIRenderer.h"
#include "Light.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Renderer.h"
#include "Physics.h"
#include "SingleRTDeferredRenderer.h"
#include "MultipleRTDeferredRenderer.h"
#include "FXAA.h"
#include "FPSCounter.h"
#include "ForwardRenderer.h"

Renderer * g_renderer = 0;

int g_rendererType = Renderer::TypeDeferredRenderer;
FontHandle g_font;
IDirect3D9 * g_d3d = 0;
IDirect3DDevice9 * g_device = 0;
IDirect3DVertexDeclaration9 * g_meshVertexDeclaration = 0;
vector< SceneNode* > g_nodes;
HWND window;

float g_width = 0;
float g_height = 0;
int g_dips = 0;
int g_debugDraw = 0;
int g_textureChanges = 0;
int g_fps = 0;
FPSCounter g_fpsCounter;
bool g_engineRunning = true;
bool g_useShadows = true;
IDirect3DTexture9 * g_renderTexture = 0;
IDirect3DSurface9 * g_renderSurface = 0;
IDirect3DSurface9 * g_backbufferSurface = 0;

Vector3 g_ambientColor = Vector3( 0.05, 0.05, 0.05 );

vector< Light*> affectedLights;

Renderer::~Renderer() {
    delete g_particleSystemRenderer;
    if( g_deferredRenderer ) {
        delete g_deferredRenderer;
    }
    if( g_guiRenderer ) {
        delete g_guiRenderer;
    }
    while( g_nodes.size() ) {
        delete g_nodes.front();
    }
    if( g_forwardRenderer ) {
        delete g_forwardRenderer;
    }
    for( auto texPair : CubeTexture::all ) {
        texPair.second->cubeTexture->Release();
    }
    Texture::DeleteAll();
    if( g_meshVertexDeclaration ) {
        g_meshVertexDeclaration->Release();
    }
    int counter = 0;
    if( g_device ) {
        while( g_device->Release() ) {
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
}

Renderer::Renderer( int width, int height, int fullscreen ) {
    if ( width == 0 ) {
        width = GetSystemMetrics ( SM_CXSCREEN );
    }
    if ( height == 0 ) {
        height = GetSystemMetrics ( SM_CYSCREEN );
    }
    if( !CreateRenderWindow( width, height, fullscreen )) {
        return;
    }

    g_d3d = Direct3DCreate9( D3D_SDK_VERSION );

    if( !g_d3d ) {
        return;
    }

    g_width = width;
    g_height = height;

    D3DDISPLAYMODE displayMode = { 0 };
    g_d3d->GetAdapterDisplayMode ( D3DADAPTER_DEFAULT, &displayMode );

    // present parameters
    D3DPRESENT_PARAMETERS presentParameters = { 0 };
    presentParameters.BackBufferCount = 1;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
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
    if ( FAILED ( g_d3d->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters, &g_device ) ) ) {
        if ( FAILED ( g_d3d->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_MIXED_VERTEXPROCESSING, &presentParameters, &g_device ) ) ) {
            if ( FAILED ( g_d3d->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &presentParameters, &g_device ) ) ) {
                MessageBoxA( 0, "Failed to create Direct3D 9 Device", "Epic fail", 0 );

                abort();

                return;
            }
        }
    }

    D3DVERTEXELEMENT9 vd[ ] = {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        { 0, 40, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        D3DDECL_END()
    };

    g_device->CreateVertexDeclaration( vd, &g_meshVertexDeclaration ) ;
    g_device->SetVertexDeclaration ( g_meshVertexDeclaration );

    g_device->SetRenderState ( D3DRS_LIGHTING, FALSE );

    g_device->SetRenderState ( D3DRS_ZENABLE, TRUE );
    g_device->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
    g_device->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

    g_device->SetRenderState ( D3DRS_ALPHAREF, 100 );
    g_device->SetRenderState ( D3DRS_ALPHATESTENABLE, TRUE );
    g_device->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

    g_device->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );

    D3DCAPS9 caps;
    g_device->GetDeviceCaps( &caps );

    unsigned char psVerHi = D3DSHADER_VERSION_MAJOR( caps.PixelShaderVersion );
    unsigned char psVerLo = D3DSHADER_VERSION_MINOR( caps.PixelShaderVersion );

    if( psVerHi < 2 ) {
        MessageBoxA( 0, "Your graphics card doesn't support Pixel Shader 2.0. Engine initialization failed! Buy a modern video card!", "Epic fail", 0 );

        abort();
    }

    g_device->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
    g_device->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    g_device->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
    g_device->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, caps.MaxAnisotropy );

    g_device->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
    g_device->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    g_device->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
    g_device->SetSamplerState ( 1, D3DSAMP_MAXANISOTROPY, caps.MaxAnisotropy );

    CreatePhysics( );
    pfSystemInit( );
    pfSetListenerDopplerFactor( 0 );
    
    if( caps.NumSimultaneousRTs < 3 ) {
        g_deferredRenderer = new SingleRTDeferredRenderer();
    } else {
        g_deferredRenderer = new MultipleRTDeferredRenderer();
    }
    g_forwardRenderer = new ForwardRenderer();
    g_particleSystemRenderer = new ParticleSystemRenderer();

    g_guiRenderer = new GUIRenderer();
    g_font = g_guiRenderer->CreateFont( 12, "Arial", 0, 0 );
    g_renderer = this;
    performanceTimer = new Timer;
}

/*
==========
Renderer::IsMeshVisible
==========
*/
bool Renderer::IsMeshVisible( Mesh * mesh ) {
    mesh->ownerNode->inFrustum = g_camera->frustum.IsAABBInside( mesh->aabb, Vector3( mesh->ownerNode->globalTransform.getOrigin().m_floats ));
    return mesh->ownerNode->skinned || mesh->ownerNode->IsVisible() && mesh->ownerNode->inFrustum;
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
    const char * className = "Mine";
    WNDCLASSEXA wcx = { 0 };
    wcx.cbSize = sizeof ( wcx );
    wcx.hCursor = LoadCursor ( NULL, IDC_ARROW );
    wcx.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
    wcx.hInstance = instance;
    wcx.lpfnWndProc = WindowProcess;
    wcx.lpszClassName = className;
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClassExA ( &wcx );    
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
    window = CreateWindowA ( className, "Mine", style, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, instance, 0 );

    if( !window ) { // fail
        return 0;
    }
    // setup window
    ShowWindow ( window, SW_SHOW );
    UpdateWindow ( window );
    SetActiveWindow ( window );
    SetForegroundWindow ( window );
    // init input
    mi::Init( &window );
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
    // set these transforms, for render passes which uses FFP
    g_device->SetTransform( D3DTS_VIEW, &g_camera->view );
    g_device->SetTransform( D3DTS_PROJECTION, &g_camera->projection );
    // precalculations
    for( auto node : g_nodes ) {
        node->CalculateGlobalTransform();
        node->PerformAnimation();
        // update all sounds attached to node, and physical interaction sounds( roll, hit )
        node->UpdateSounds();
        // skip frustum flag, it will be set to true, if one of node's mesh
        // are in frustum
        node->inFrustum = false;
    }
    // begin dx scene
    g_device->BeginScene();
    // begin rendering into G-Buffer
    g_deferredRenderer->BeginFirstPass();
    // render from current camera
    SingleRTDeferredRenderer * singleRT = dynamic_cast< SingleRTDeferredRenderer* >( g_deferredRenderer );
    if( !singleRT ) { // multiple RT's rendering
        RenderMeshesIntoGBuffer();
    } else { // single RT for poor videocards
        // diffuse pass
        singleRT->SetDiffusePass();
        RenderMeshesIntoGBuffer();
        // normal pass
        singleRT->SetNormalPass();
        RenderMeshesIntoGBuffer();
        // depth pass
        singleRT->SetDepthPass();
        RenderMeshesIntoGBuffer();
    }  
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
    g_device->EndScene();
    g_device->Present( 0, 0, 0, 0 );
    // grab info about node's physic contacts 
    SceneNode::UpdateContacts( );
    // update sound subsystem
    pfSystemUpdate();
    // update physics subsystem
    if( g_physicsEnabled ) {
        const float timeStep = 1.0f / 60.0f;
        const int subSteps = 4;
        g_dynamicsWorld->stepSimulation( timeStep, subSteps );
    }
}

/*
===============
Renderer::RenderMeshesIntoGBuffer

all registered meshes are sorted by texture, so rendering becomes really fast - there no redundant texture changes
===============
*/
void Renderer::RenderMeshesIntoGBuffer() {
    for( auto groupIterator : Mesh::meshes ) {
        IDirect3DTexture9 * diffuseTexture = groupIterator.first;
        IDirect3DTexture9 * normalTexture = nullptr;
        auto & meshes = groupIterator.second;
        // skip group if it has no meshes
        if( meshes.size() == 0 ) {
            continue;
        }
        // bind diffuse texture
        g_device->SetTexture( 0, diffuseTexture );
        // each group has same texture
        g_textureChanges++;
        for( auto meshIterator : meshes ) {
            Mesh * mesh = meshIterator;
            SceneNode * node = mesh->ownerNode;
            // prevent overhead with normal texture       
            if( mesh->GetNormalTexture() ) {
                IDirect3DTexture9 * meshNormalTexture = mesh->GetNormalTexture()->GetInterface();
                if( meshNormalTexture != normalTexture ) {
                    mesh->GetNormalTexture()->Bind( 1 );
                    normalTexture = meshNormalTexture;
                }
            }
            if( IsMeshVisible( mesh ) ) {
                if( !mesh->indexBuffer || !mesh->vertexBuffer ) {
                    continue;
                }
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
void Renderer::UpdateMessagePump() {
    MSG message;

    while ( PeekMessage ( &message, NULL, 0, 0, PM_REMOVE ) ) {
        DispatchMessage ( &message );

        if ( message.message == WM_QUIT ) {
            FreeRenderer();
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
        FreeRenderer();
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
DebugDrawEnabled
===============
*/
void DebugDrawEnabled( int state ) {
    g_debugDraw = state;
}

/*
===============
SetTextureFiltering
===============
*/
void SetTextureFiltering( const int & filter, int anisotropicQuality ) {
    int minMagFilter = D3DTEXF_POINT;

    if( filter == TextureFilter::Nearest ) {
        minMagFilter = D3DTEXF_POINT;
    }
    if( filter == TextureFilter::Linear ) {
        minMagFilter = D3DTEXF_LINEAR;
    }
    if( filter == TextureFilter::Anisotropic ) {
        minMagFilter = D3DTEXF_ANISOTROPIC;
    }
    int mipFilter = D3DTEXF_LINEAR;

    if( mipFilter == TextureFilter::Nearest ) {
        mipFilter = D3DTEXF_POINT;
    }
    g_device->SetSamplerState ( 0, D3DSAMP_MINFILTER, minMagFilter );
    g_device->SetSamplerState ( 0, D3DSAMP_MIPFILTER, mipFilter );
    g_device->SetSamplerState ( 0, D3DSAMP_MAGFILTER, minMagFilter );
    g_device->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, anisotropicQuality );

    g_device->SetSamplerState ( 1, D3DSAMP_MINFILTER, minMagFilter );
    g_device->SetSamplerState ( 1, D3DSAMP_MIPFILTER, mipFilter );
    g_device->SetSamplerState ( 1, D3DSAMP_MAGFILTER, minMagFilter );
    g_device->SetSamplerState ( 1, D3DSAMP_MAXANISOTROPY, anisotropicQuality );
}
/*
===============
DIPs
===============
*/
int DIPs( ) {
    return g_dips;
}
/*
===============
CreateRenderer
===============
*/
int CreateRenderer( int width, int height, int fullscreen ) {
    Renderer * renderer = new Renderer( width, height, fullscreen ) ;
    return 1;
}

/*
===============
SetAmbientColor
===============
*/
void SetAmbientColor( Vector3 color ) {
    g_ambientColor = color;
}

/*
===============
GetAvailableTextureMemory
===============
*/
int GetAvailableTextureMemory() {
    return g_device->GetAvailableTextureMem();
}

/*
===============
RayPick
===============
*/
NodeHandle RayPick( int x, int y, Vector3 * outPickPoint ) {
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
int GetMaxAnisotropy() {
    D3DCAPS9 caps;
    g_device->GetDeviceCaps( &caps );

    return caps.MaxAnisotropy;
}

/*
===============
FreeRenderer
===============
*/
int FreeRenderer( ) {
    g_engineRunning = false;
    delete g_renderer;
    return 1;
}

/*
===============
GetResolutionWidth
===============
*/
int GetResolutionWidth( ) {
    return g_width;
}

/*
===============
GetResolutionHeight
===============
*/
int GetResolutionHeight( ) {
    return g_height;
}

/*
===============
TextureUsedPerFrame
===============
*/
int TextureUsedPerFrame( ) {
    return g_textureChanges;
}

/*
===============
RenderWorld
===============
*/
int RenderWorld( float dt ) {
    g_renderer->RenderWorld();

    return 1;
}

/*
===============
EnableShadows
===============
*/
API void EnableShadows( bool state ) {
    g_useShadows = state;
}