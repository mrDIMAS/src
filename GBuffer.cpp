#include "Precompiled.h"
#include "Engine.h"
#include "GBuffer.h"
#include "Utility.h"

GBuffer::GBuffer() {
	CreateRenderTargets();
}

GBuffer::~GBuffer() {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, backSurface );
    Engine::Instance().GetDevice()->SetRenderTarget( 1, 0 );
    Engine::Instance().GetDevice()->SetRenderTarget( 2, 0 );
    FreeRenderTargets();
}

void GBuffer::BindRenderTargets() {
    UnbindTextures();

    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderTarget( 0, depthSurface ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderTarget( 1, normalSurface ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderTarget( 2, diffuseSurface ));
}

void GBuffer::BindNormalMapAsRT() {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderTarget( 0, normalSurface ));
};

void GBuffer::BindDiffuseMapAsRT() {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderTarget( 0, diffuseSurface ));
};

void GBuffer::BindDepthMapAsRT() {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderTarget( 0, depthSurface ));
};

void GBuffer::UnbindTextures() {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTexture( 0, 0 ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTexture( 1, 0 ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTexture( 2, 0 ));
};

void GBuffer::UnbindRenderTargets() {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderTarget( 0, backSurface ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderTarget( 1, 0 ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderTarget( 2, 0 ));
}

void GBuffer::BindBackSurfaceAsRT() {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetRenderTarget( 0, backSurface ));
};

void GBuffer::BindTextures() {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTexture( 0, depthMap ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTexture( 1, normalMap ));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTexture( 2, diffuseMap ));
}

void GBuffer::BindDepthMap( int layer ) {
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTexture( layer, depthMap ));
}

void GBuffer::OnDeviceLost() {
	FreeRenderTargets();
	CreateRenderTargets();
}

void GBuffer::FreeRenderTargets() {
	depthSurface->Release();
	normalSurface->Release();
	diffuseSurface->Release();
	backSurface->Release();
	while( depthMap->Release() );
	while( normalMap->Release());
	while( diffuseMap->Release());
}

void GBuffer::CreateRenderTargets() {
	int width = Engine::Instance().GetResolutionWidth();
	int height = Engine::Instance().GetResolutionHeight();
	if( !Engine::Instance().IsFullNPOTTexturesSupport()) {
		width = NearestPow2( Engine::Instance().GetResolutionWidth() );
		height = NearestPow2( Engine::Instance().GetResolutionHeight() );
	}

	CheckDXErrorFatal( D3DXCreateTexture( Engine::Instance().GetDevice(), width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &depthMap ));
	CheckDXErrorFatal(  D3DXCreateTexture( Engine::Instance().GetDevice(), width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &normalMap ));
	CheckDXErrorFatal(  D3DXCreateTexture( Engine::Instance().GetDevice(), width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &diffuseMap ));
	CheckDXErrorFatal( depthMap->GetSurfaceLevel( 0, &depthSurface ));
	CheckDXErrorFatal( normalMap->GetSurfaceLevel( 0, &normalSurface ));
	CheckDXErrorFatal( diffuseMap->GetSurfaceLevel( 0, &diffuseSurface ));
	CheckDXErrorFatal( Engine::Instance().GetDevice()->GetRenderTarget( 0, &backSurface ));
}
