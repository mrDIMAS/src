#include "GBuffer.h"

GBuffer::GBuffer() {
    //int width = GetNearestPow2( g_width );
    //int height = GetNearestPow2( g_height );
    int width = g_width;
    int height = g_height;

#ifdef USE_R32F_DEPTH
    if( FAILED( D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &depthMap )))
#else
    if( FAILED( D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &depthMap )))
#endif

        MessageBoxA( 0, "Failed to create 'Depth Map' texture.", 0, MB_OK | MB_ICONERROR );

    if( FAILED( D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &normalMap ))) {
        MessageBoxA( 0, "Failed to create 'Normal Map' texture.", 0, MB_OK | MB_ICONERROR );
    }
    if( FAILED( D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &diffuseMap ))) {
        MessageBoxA( 0, "Failed to create 'Diffuse Map' texture.", 0, MB_OK | MB_ICONERROR );
    }

    depthMap->GetSurfaceLevel( 0, &depthSurface );
    normalMap->GetSurfaceLevel( 0, &normalSurface );
    diffuseMap->GetSurfaceLevel( 0, &diffuseSurface );

    g_device->GetRenderTarget( 0, &backSurface );
}

GBuffer::~GBuffer() {
    if( depthMap ) {
        depthMap->Release();
    }
    if( normalMap ) {
        normalMap->Release();
    }
    if( diffuseMap ) {
        diffuseMap->Release();
    }

    if( depthSurface ) {
        depthSurface->Release();
    }
    if( normalSurface ) {
        normalSurface->Release();
    }
    if( diffuseSurface ) {
        diffuseSurface->Release();
    }
}

void GBuffer::BindRenderTargets() {
    UnbindTextures();

    g_device->SetRenderTarget( 0, depthSurface );
    g_device->SetRenderTarget( 1, normalSurface );
    g_device->SetRenderTarget( 2, diffuseSurface );
}

void GBuffer::BindNormalMapAsRT() {
    g_device->SetRenderTarget( 0, normalSurface );
};

void GBuffer::BindDiffuseMapAsRT() {
    g_device->SetRenderTarget( 0, diffuseSurface );
};

void GBuffer::BindDepthMapAsRT() {
    g_device->SetRenderTarget( 0, depthSurface );
};

void GBuffer::UnbindTextures() {
    g_device->SetTexture( 0, 0 );
    g_device->SetTexture( 1, 0 );
    g_device->SetTexture( 2, 0 );
};

void GBuffer::UnbindRenderTargets() {
    g_device->SetRenderTarget( 0, backSurface );
    g_device->SetRenderTarget( 1, 0 );
    g_device->SetRenderTarget( 2, 0 );
}

void GBuffer::BindBackSurfaceAsRT() {
    g_device->SetRenderTarget( 0, backSurface );
};

void GBuffer::BindTextures() {
    g_device->SetTexture( 0, depthMap );
    g_device->SetTexture( 1, normalMap );
    g_device->SetTexture( 2, diffuseMap );
}

void GBuffer::BindDepthMap( int layer ) {
    g_device->SetTexture( layer, depthMap );
}