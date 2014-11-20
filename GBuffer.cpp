#include "GBuffer.h"

GBuffer::GBuffer() {
    //int width = GetNearestPow2( g_width );
    //int height = GetNearestPow2( g_height );
    int width = g_width;
    int height = g_height;

#ifdef USE_R32F_DEPTH
    CheckDXError( D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &depthMap ));
#else
    CheckDXError( D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &depthMap )));
#endif
    CheckDXError(  D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &normalMap ));
    CheckDXError(  D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &diffuseMap ));
    CheckDXError( depthMap->GetSurfaceLevel( 0, &depthSurface ));
    CheckDXError( normalMap->GetSurfaceLevel( 0, &normalSurface ));
    CheckDXError( diffuseMap->GetSurfaceLevel( 0, &diffuseSurface ));
    CheckDXError( g_device->GetRenderTarget( 0, &backSurface ));
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

    CheckDXError( g_device->SetRenderTarget( 0, depthSurface ));
    CheckDXError( g_device->SetRenderTarget( 1, normalSurface ));
    CheckDXError( g_device->SetRenderTarget( 2, diffuseSurface ));
}

void GBuffer::BindNormalMapAsRT() {
    CheckDXError( g_device->SetRenderTarget( 0, normalSurface ));
};

void GBuffer::BindDiffuseMapAsRT() {
    CheckDXError( g_device->SetRenderTarget( 0, diffuseSurface ));
};

void GBuffer::BindDepthMapAsRT() {
    CheckDXError( g_device->SetRenderTarget( 0, depthSurface ));
};

void GBuffer::UnbindTextures() {
    CheckDXError( g_device->SetTexture( 0, 0 ));
    CheckDXError( g_device->SetTexture( 1, 0 ));
    CheckDXError( g_device->SetTexture( 2, 0 ));
};

void GBuffer::UnbindRenderTargets() {
    CheckDXError( g_device->SetRenderTarget( 0, backSurface ));
    CheckDXError( g_device->SetRenderTarget( 1, 0 ));
    CheckDXError( g_device->SetRenderTarget( 2, 0 ));
}

void GBuffer::BindBackSurfaceAsRT() {
    CheckDXError( g_device->SetRenderTarget( 0, backSurface ));
};

void GBuffer::BindTextures() {
    CheckDXError( g_device->SetTexture( 0, depthMap ));
    CheckDXError( g_device->SetTexture( 1, normalMap ));
    CheckDXError( g_device->SetTexture( 2, diffuseMap ));
}

void GBuffer::BindDepthMap( int layer ) {
    CheckDXError( g_device->SetTexture( layer, depthMap ));
}