#include "GBuffer.h"

GBuffer::GBuffer() {
    //int width = GetNearestPow2( g_width );
    //int height = GetNearestPow2( g_height );
    int width = g_width;
    int height = g_height;

#ifdef USE_R32F_DEPTH
    CheckDXErrorFatal( D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &depthMap ));
#else
    CheckDXErrorFatal( D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &depthMap )));
#endif
    CheckDXErrorFatal(  D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &normalMap ));
    CheckDXErrorFatal(  D3DXCreateTexture( g_device, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &diffuseMap ));
    CheckDXErrorFatal( depthMap->GetSurfaceLevel( 0, &depthSurface ));
    CheckDXErrorFatal( normalMap->GetSurfaceLevel( 0, &normalSurface ));
    CheckDXErrorFatal( diffuseMap->GetSurfaceLevel( 0, &diffuseSurface ));
    CheckDXErrorFatal( g_device->GetRenderTarget( 0, &backSurface ));
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

    CheckDXErrorFatal( g_device->SetRenderTarget( 0, depthSurface ));
    CheckDXErrorFatal( g_device->SetRenderTarget( 1, normalSurface ));
    CheckDXErrorFatal( g_device->SetRenderTarget( 2, diffuseSurface ));
}

void GBuffer::BindNormalMapAsRT() {
    CheckDXErrorFatal( g_device->SetRenderTarget( 0, normalSurface ));
};

void GBuffer::BindDiffuseMapAsRT() {
    CheckDXErrorFatal( g_device->SetRenderTarget( 0, diffuseSurface ));
};

void GBuffer::BindDepthMapAsRT() {
    CheckDXErrorFatal( g_device->SetRenderTarget( 0, depthSurface ));
};

void GBuffer::UnbindTextures() {
    CheckDXErrorFatal( g_device->SetTexture( 0, 0 ));
    CheckDXErrorFatal( g_device->SetTexture( 1, 0 ));
    CheckDXErrorFatal( g_device->SetTexture( 2, 0 ));
};

void GBuffer::UnbindRenderTargets() {
    CheckDXErrorFatal( g_device->SetRenderTarget( 0, backSurface ));
    CheckDXErrorFatal( g_device->SetRenderTarget( 1, 0 ));
    CheckDXErrorFatal( g_device->SetRenderTarget( 2, 0 ));
}

void GBuffer::BindBackSurfaceAsRT() {
    CheckDXErrorFatal( g_device->SetRenderTarget( 0, backSurface ));
};

void GBuffer::BindTextures() {
    CheckDXErrorFatal( g_device->SetTexture( 0, depthMap ));
    CheckDXErrorFatal( g_device->SetTexture( 1, normalMap ));
    CheckDXErrorFatal( g_device->SetTexture( 2, diffuseMap ));
}

void GBuffer::BindDepthMap( int layer ) {
    CheckDXErrorFatal( g_device->SetTexture( layer, depthMap ));
}