#include "GBuffer.h"

GBuffer::GBuffer() {
	int width = g_width;
	int height = g_height;
	if( !IsFullNPOTTexturesSupport()) {
		width = NearestPow2( g_width );
		height = NearestPow2( g_height );
	}

#ifdef USE_R32F_DEPTH
    CheckDXErrorFatal( D3DXCreateTexture( gpDevice, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &depthMap ));
#else
    CheckDXErrorFatal( D3DXCreateTexture( gpDevice, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &depthMap )));
#endif
    CheckDXErrorFatal(  D3DXCreateTexture( gpDevice, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &normalMap ));
    CheckDXErrorFatal(  D3DXCreateTexture( gpDevice, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &diffuseMap ));
    CheckDXErrorFatal( depthMap->GetSurfaceLevel( 0, &depthSurface ));
    CheckDXErrorFatal( normalMap->GetSurfaceLevel( 0, &normalSurface ));
    CheckDXErrorFatal( diffuseMap->GetSurfaceLevel( 0, &diffuseSurface ));
    CheckDXErrorFatal( gpDevice->GetRenderTarget( 0, &backSurface ));
}

GBuffer::~GBuffer() {
    gpDevice->SetRenderTarget( 0, backSurface );
    gpDevice->SetRenderTarget( 1, 0 );
    gpDevice->SetRenderTarget( 2, 0 );
    depthSurface->Release();
    normalSurface->Release();
    diffuseSurface->Release();
    backSurface->Release();
    while( depthMap->Release() );
    while( normalMap->Release());
    while( diffuseMap->Release());
}

void GBuffer::BindRenderTargets() {
    UnbindTextures();

    CheckDXErrorFatal( gpDevice->SetRenderTarget( 0, depthSurface ));
    CheckDXErrorFatal( gpDevice->SetRenderTarget( 1, normalSurface ));
    CheckDXErrorFatal( gpDevice->SetRenderTarget( 2, diffuseSurface ));
}

void GBuffer::BindNormalMapAsRT() {
    CheckDXErrorFatal( gpDevice->SetRenderTarget( 0, normalSurface ));
};

void GBuffer::BindDiffuseMapAsRT() {
    CheckDXErrorFatal( gpDevice->SetRenderTarget( 0, diffuseSurface ));
};

void GBuffer::BindDepthMapAsRT() {
    CheckDXErrorFatal( gpDevice->SetRenderTarget( 0, depthSurface ));
};

void GBuffer::UnbindTextures() {
    CheckDXErrorFatal( gpDevice->SetTexture( 0, 0 ));
    CheckDXErrorFatal( gpDevice->SetTexture( 1, 0 ));
    CheckDXErrorFatal( gpDevice->SetTexture( 2, 0 ));
};

void GBuffer::UnbindRenderTargets() {
    CheckDXErrorFatal( gpDevice->SetRenderTarget( 0, backSurface ));
    CheckDXErrorFatal( gpDevice->SetRenderTarget( 1, 0 ));
    CheckDXErrorFatal( gpDevice->SetRenderTarget( 2, 0 ));
}

void GBuffer::BindBackSurfaceAsRT() {
    CheckDXErrorFatal( gpDevice->SetRenderTarget( 0, backSurface ));
};

void GBuffer::BindTextures() {
    CheckDXErrorFatal( gpDevice->SetTexture( 0, depthMap ));
    CheckDXErrorFatal( gpDevice->SetTexture( 1, normalMap ));
    CheckDXErrorFatal( gpDevice->SetTexture( 2, diffuseMap ));
}

void GBuffer::BindDepthMap( int layer ) {
    CheckDXErrorFatal( gpDevice->SetTexture( layer, depthMap ));
}