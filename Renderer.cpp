#include "Common.h"
#include "ParticleSystemRenderer.h"
#include "LightmapRenderer.h"
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

Renderer * g_renderer = 0;

int g_rendererType = Renderer::TypeDeferredRenderer;
FontHandle g_font;
IDirect3D9 * g_d3d = 0;
IDirect3DDevice9 * g_device = 0;
IDirect3DVertexDeclaration9 * g_meshVertexDeclaration = 0;
vector< SceneNode* > g_nodes;
HWND window;

int g_width = 0;
int g_height = 0;
int g_dips = 0;
int g_debugDraw = 0;
int g_textureChanges = 0;
int g_fps = 0;
FPSCounter g_fpsCounter;

IDirect3DTexture9 * g_renderTexture = 0;
IDirect3DSurface9 * g_renderSurface = 0;
IDirect3DSurface9 * g_backbufferSurface = 0;

Vector3 g_ambientColor = Vector3( 0.05, 0.05, 0.05 );

vector< Light*> affectedLights;

Renderer::~Renderer() {
    delete g_particleSystemRenderer;

    if( g_deferredRenderer )
        delete g_deferredRenderer;

    if( g_guiRenderer )
        delete g_guiRenderer;

    while( g_nodes.size() )
        delete g_nodes.front();

    Texture::DeleteAll();

    if( g_meshVertexDeclaration ) {
        g_meshVertexDeclaration->Release();
        g_meshVertexDeclaration = 0;
    }

    if( g_device ) {
        while( g_device->Release() );

        g_device = 0;
    }

    if( g_d3d ) {
        while( g_d3d->Release() );

        g_d3d = 0;
    }

    Physics::DestructWorld();

    pfSystemDestroy();
}

Renderer::Renderer( int width, int height, int fullscreen ) {
    if ( width == 0 )
        width = GetSystemMetrics ( SM_CXSCREEN );

    if ( height == 0 )
        height = GetSystemMetrics ( SM_CYSCREEN );

    if( !CreateRenderWindow( width, height, fullscreen ))
        return;

    g_d3d = Direct3DCreate9( D3D_SDK_VERSION );

    if( !g_d3d )
        return;

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
    }
    else {
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

    if( g_rendererType == Renderer::TypeLightMapRenderer ) {
        g_lightMapRenderer = new LightmapRenderer;
    }
    else if( g_rendererType == Renderer::TypeDeferredRenderer ) {
        D3DCAPS9 caps;
        g_device->GetDeviceCaps( &caps );

        if( caps.NumSimultaneousRTs < 3 )
            g_deferredRenderer = new SingleRTDeferredRenderer();
        else
            g_deferredRenderer = new MultipleRTDeferredRenderer();

        g_particleSystemRenderer = new ParticleSystemRenderer();
    }

    g_guiRenderer = new GUIRenderer();

    g_font = g_guiRenderer->CreateFont( 12, "Arial", 0, 0 );

    g_renderer = this;

    performanceTimer = new Timer;
}

void SetTextureFiltering( const int & filter, int anisotropicQuality ) {
    int minMagFilter = D3DTEXF_POINT;

    if( filter == TextureFilter::Nearest )
        minMagFilter = D3DTEXF_POINT;
    if( filter == TextureFilter::Linear )
        minMagFilter = D3DTEXF_LINEAR;
    if( filter == TextureFilter::Anisotropic )
        minMagFilter = D3DTEXF_ANISOTROPIC;

    int mipFilter = D3DTEXF_LINEAR;

    if( mipFilter == TextureFilter::Nearest )
        mipFilter = D3DTEXF_POINT;

    g_device->SetSamplerState ( 0, D3DSAMP_MINFILTER, minMagFilter );
    g_device->SetSamplerState ( 0, D3DSAMP_MIPFILTER, mipFilter );
    g_device->SetSamplerState ( 0, D3DSAMP_MAGFILTER, minMagFilter );
    g_device->SetSamplerState ( 0, D3DSAMP_MAXANISOTROPY, anisotropicQuality );

    g_device->SetSamplerState ( 1, D3DSAMP_MINFILTER, minMagFilter );
    g_device->SetSamplerState ( 1, D3DSAMP_MIPFILTER, mipFilter );
    g_device->SetSamplerState ( 1, D3DSAMP_MAGFILTER, minMagFilter );
    g_device->SetSamplerState ( 1, D3DSAMP_MAXANISOTROPY, anisotropicQuality );
}

int GetMaxAnisotropy() {
    D3DCAPS9 caps;
    g_device->GetDeviceCaps( &caps );

    return caps.MaxAnisotropy;
}



bool Renderer::PointInBV( BoundingVolume bv, Vector3 point ) {
    if( point.x > bv.min.x && point.x < bv.max.x &&
            point.y > bv.min.y && point.y < bv.max.y &&
            point.z > bv.min.z && point.z < bv.max.z )
        return true;
    return false;
}

bool Renderer::IsLightVisible( Light * lit ) {
    D3DXVECTOR3 snPos;

    btVector3 pos = lit->globalTransform.getOrigin();

    snPos.x = pos.x();
    snPos.y = pos.y();
    snPos.z = pos.z();

    for ( int i = 0; i < 6; i++ )
        if ( D3DXPlaneDotCoord ( &g_camera->frustumPlanes[i], &snPos ) + lit->GetRadius() < 0 )
            return FALSE;

    return true;
}

bool Renderer::IsMeshVisible( Mesh * mesh ) {
    if( mesh->parent->skinned )
        return true;

    SceneNode * node = mesh->parent;

    btVector3 pos = node->globalTransform.getOrigin();
    D3DXVECTOR3 snPos;
    snPos.x = pos.x() + mesh->boundingVolume.center.x;
    snPos.y = pos.y() + mesh->boundingVolume.center.y;
    snPos.z = pos.z() + mesh->boundingVolume.center.z;

    for ( int i = 0; i < 6; i++ )
        if ( D3DXPlaneDotCoord ( &g_camera->frustumPlanes[i], &snPos ) + mesh->boundingVolume.radius < 0 )
            return false;

    node->inFrustum = true;

    return node->IsVisible();
}

bool Renderer::SortByTexture( Mesh * mesh1, Mesh * mesh2 ) {
    return mesh1->diffuseTexture->GetInterface() < mesh2->diffuseTexture->GetInterface();
}

int Renderer::CreateRenderWindow( int width, int height, int fullscreen ) {
    HINSTANCE instance = GetModuleHandle ( 0 );

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

    DWORD style = WS_POPUP;

    if ( !fullscreen )
        style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

    RECT wRect;
    wRect.left = 0;
    wRect.top = 0;
    wRect.right = width;
    wRect.bottom = height;

    AdjustWindowRect ( &wRect, style, 0 );

    window = CreateWindowA ( className, "Mine", style, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, instance, 0 );

    if( !window )
        return 0;

    ShowWindow ( window, SW_SHOW );
    UpdateWindow ( window );
    SetActiveWindow ( window );
    SetForegroundWindow ( window );

    mi::Init( &window );

    return 1;
}

void Renderer::CreatePhysics() {
    Physics::CreateWorld();
}

void Renderer::RenderWorld() {
    g_fpsCounter.RegisterFrame();

    SceneNode::EraseUnusedNodes();

    UpdateMessagePump();

    // grab some statistics
    g_dips = 0;
    g_textureChanges = 0;



    if( g_camera ) {
        g_device->SetTransform( D3DTS_VIEW, &g_camera->view );
        g_device->SetTransform( D3DTS_PROJECTION, &g_camera->projection );

        g_camera->Update();
        g_camera->BuildFrustum();
    }


    // precalculations
    for( auto node : g_nodes ) {
        node->CalculateGlobalTransform();
        node->PerformAnimation();
        node->UpdateSounds();
        node->inFrustum = false;
    }

    g_device->BeginScene();
    g_deferredRenderer->BeginFirstPass();

    if( g_camera ) {
        SingleRTDeferredRenderer * singleRT = dynamic_cast< SingleRTDeferredRenderer* >( g_deferredRenderer );

        if( !singleRT ) {
            RenderMeshesIntoGBuffer();
        }
        else {
            singleRT->SetDiffusePass();
            RenderMeshesIntoGBuffer();

            singleRT->SetNormalPass();
            RenderMeshesIntoGBuffer();

            singleRT->SetDepthPass();
            RenderMeshesIntoGBuffer();
        }
    }

    g_deferredRenderer->EndFirstPassAndDoSecondPass();
    g_particleSystemRenderer->RenderAllParticleSystems();
    g_guiRenderer->RenderAllGUIElements();

    g_device->EndScene();
    g_device->Present( 0, 0, 0, 0 );

    SceneNode::UpdateContacts( );
    pfSystemUpdate();
    float timeStep = 1.0f / 60.0f;

    //if( g_fpsCounter.fps > 0 )
    //  timeStep = 1.0f / (float)g_fpsCounter.fps;

    const int subSteps = 4;

    if( g_physicsEnabled )
        g_dynamicsWorld->stepSimulation ( timeStep, subSteps );
}

void Renderer::LightmapRenderMeshByGroups() {
    for( auto groupIterator : Mesh::meshes ) {
        IDirect3DTexture9 * texture = groupIterator.first;
        auto & meshes = groupIterator.second;

        if( meshes.size() == 0 )
            continue;

        // bind diffuse texture
        g_device->SetTexture( 0, texture );

        g_textureChanges++;

        for( auto meshIterator : meshes ) {
            Mesh * mesh = meshIterator;
            SceneNode * node = mesh->parent;

            if( IsMeshVisible( mesh )) {
                if( !mesh->indexBuffer )
                    continue;

                if( !mesh->vertexBuffer )
                    continue;

                g_dips++;

                mesh->lightMapTexture->Bind( 1 );

                g_lightMapRenderer->RenderMesh( mesh );
            }
        }
    }
}

void Renderer::RenderMeshesIntoGBuffer() {
    for( auto groupIterator : Mesh::meshes ) {
        IDirect3DTexture9 * texture = groupIterator.first;
        auto & meshes = groupIterator.second;

        if( meshes.size() == 0 )
            continue;

        // bind diffuse texture
        g_device->SetTexture( 0, texture );

        g_textureChanges++;

        for( auto meshIterator : meshes ) {
            Mesh * mesh = meshIterator;
            SceneNode * node = mesh->parent;

            if( mesh->GetNormalTexture() )
                mesh->GetNormalTexture()->Bind( 1 );

            if( IsMeshVisible( mesh ) ) {
                if( !mesh->indexBuffer )
                    continue;

                if( !mesh->vertexBuffer )
                    continue;

                g_dips++;

                g_deferredRenderer->RenderMesh( mesh );
            }
        }
    }
}

void Renderer::UpdateMessagePump() {
    MSG message;

    while ( PeekMessage ( &message, NULL, 0, 0, PM_REMOVE ) ) {
        DispatchMessage ( &message );

        if ( message.message == WM_QUIT ) {
            FreeRenderer();
        }
    }
}

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
void DebugDrawEnabled( int state ) {
    g_debugDraw = state;
}

int DIPs( ) {
    return g_dips;
}

int CreateRenderer( int width, int height, int fullscreen ) {
    Renderer * renderer = new Renderer( width, height, fullscreen ) ;
    return 1;
}

void SetAmbientColor( Vector3 color ) {
    g_ambientColor = color;
}

int GetAvailableTextureMemory() {
    return g_device->GetAvailableTextureMem();
}

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

    D3DXMATRIX matinv;
    g_device->GetTransform ( D3DTS_VIEW, &matinv );
    D3DXMatrixInverse ( &matinv, NULL, &matinv );

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

int FreeRenderer( ) {
    delete g_renderer;
    return 1;
}

int GetResolutionWidth( ) {
    return g_width;
}

int GetResolutionHeight( ) {
    return g_height;
}

int TextureUsedPerFrame( ) {
    return g_textureChanges;
}

int RenderWorld( float dt ) {
    g_renderer->RenderWorld();

    return 1;
}