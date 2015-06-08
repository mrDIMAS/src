#include "Precompiled.h"
#include "Engine.h"
#include "HDR.h"
#include "Utility.h"

void HDRShader::DoToneMapping( IDirect3DSurface9 * targetSurface ) {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, targetSurface );
    Engine::Instance().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
    Engine::Instance().GetDevice()->SetTexture( 0, hdrTexture );
    Engine::Instance().GetDevice()->SetTexture( 7, adaptedLuminanceCurrent );
    toneMapShader->Bind();
    screenQuad->Bind();
    screenQuad->Render();
}

void HDRShader::SetAsRenderTarget() {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, hdrSurface );
}

HDRShader::~HDRShader() {
	OnLostDevice();
    delete screenQuad;
    delete toneMapShader;
    delete scaleScenePixelShader;
    delete adaptationPixelShader;
    delete downScalePixelShader;
}

HDRShader::HDRShader() {
	OnResetDevice();
	
	toneMapShader = new PixelShader( "data/shaders/hdrTonemap.pso" );
	screenQuad = new EffectsQuad( false );
    scaleScenePixelShader = new PixelShader( "data/shaders/hdrScale.pso" );
    adaptationPixelShader = new PixelShader( "data/shaders/hdrAdaptation.pso" );
	downScalePixelShader = new PixelShader( "data/shaders/hdrDownscale.pso" );
}

void HDRShader::CalculateFrameLuminance( ) {
    screenQuad->Bind();

    Engine::Instance().GetDevice()->SetTexture( 7, hdrTexture );
    Engine::Instance().GetDevice()->SetRenderTarget( 0, scaledSceneSurf );
    scaleScenePixelShader->Bind();
    screenQuad->Render();

    // next, do downsampling until we get 1x1 luminance texture
    downScalePixelShader->Bind();
    // 256x256
    Engine::Instance().GetDevice()->SetTexture( 7, scaledScene );
    Engine::Instance().GetDevice()->SetRenderTarget( 0, downSampSurf[ 0 ] );
    Engine::Instance().SetPixelShaderFloat( 0, 1.0f / 256.0f );
    screenQuad->Render();
    // 128x128
    Engine::Instance().GetDevice()->SetTexture( 7, downSampTex[ 0 ] );
    Engine::Instance().GetDevice()->SetRenderTarget( 0, downSampSurf[ 1 ] );
    Engine::Instance().SetPixelShaderFloat( 0, 1.0f / 128.0f );
    screenQuad->Render();
    // 64x64
    Engine::Instance().GetDevice()->SetTexture( 7, downSampTex[ 1 ] );
    Engine::Instance().GetDevice()->SetRenderTarget( 0, downSampSurf[ 2 ] );
    Engine::Instance().SetPixelShaderFloat( 0, 1.0f / 64.0f );
    screenQuad->Render();
    // 32x32
    Engine::Instance().GetDevice()->SetTexture( 7, downSampTex[ 2 ] );
    Engine::Instance().GetDevice()->SetRenderTarget( 0, downSampSurf[ 3 ] );
    Engine::Instance().SetPixelShaderFloat( 0, 1.0f / 32.0f );
    screenQuad->Render();
    // 16x16
    Engine::Instance().GetDevice()->SetTexture( 7, downSampTex[ 3 ] );
    Engine::Instance().GetDevice()->SetRenderTarget( 0, downSampSurf[ 4 ] );
    Engine::Instance().SetPixelShaderFloat( 0, 1.0f / 16.0f );
    screenQuad->Render();
    // 8x8
    Engine::Instance().GetDevice()->SetTexture( 7, downSampTex[ 4 ] );
    Engine::Instance().GetDevice()->SetRenderTarget( 0, downSampSurf[ 5 ] );
    Engine::Instance().SetPixelShaderFloat( 0, 1.0f / 8.0f );
    screenQuad->Render();
    // 4x4
    Engine::Instance().GetDevice()->SetTexture( 7, downSampTex[ 5 ] );
    Engine::Instance().GetDevice()->SetRenderTarget( 0, downSampSurf[ 6 ] );
    Engine::Instance().SetPixelShaderFloat( 0, 1.0f / 4.0f );
    screenQuad->Render();
    // 2x2
    Engine::Instance().GetDevice()->SetTexture( 7, downSampTex[ 6 ] );
    Engine::Instance().GetDevice()->SetRenderTarget( 0, downSampSurf[ 7 ] );
    Engine::Instance().SetPixelShaderFloat( 0, 1.0f / 2.0f );
    screenQuad->Render();
    // final 1x1
    Engine::Instance().GetDevice()->SetTexture( 7, downSampTex[ 7 ] );
    Engine::Instance().GetDevice()->SetRenderTarget( 0, downSampSurf[ 8 ] );
    Engine::Instance().SetPixelShaderFloat( 0, 1.0f );
    screenQuad->Render();
    // now we get average frame luminance presented as 1x1 pixel RGBA8 texture
    // render it into R32F luminance texture
    for( int i = 0; i < 8; i++ ) {
        Engine::Instance().GetDevice()->SetTexture( i, 0 );
    }

    IDirect3DTexture9 * pTexSwap = adaptedLuminanceLast;
    adaptedLuminanceLast = adaptedLuminanceCurrent;
    adaptedLuminanceCurrent = pTexSwap;

    IDirect3DSurface9 * pSurfAdaptedLum = NULL;
    adaptedLuminanceCurrent->GetSurfaceLevel( 0, &pSurfAdaptedLum );

    Engine::Instance().GetDevice()->SetRenderTarget( 0, pSurfAdaptedLum );
    Engine::Instance().GetDevice()->SetTexture( 6, adaptedLuminanceLast );
    Engine::Instance().GetDevice()->SetTexture( 7, downSampTex[8] );

    adaptationPixelShader->Bind();
	Engine::Instance().SetPixelShaderFloat( 0, 0.75f ); // adaptation
    screenQuad->Render();

    pSurfAdaptedLum->Release();
}

void HDRShader::OnLostDevice()
{
	scaledSceneSurf->Release();
	scaledScene->Release();
	hdrSurface->Release();
	hdrTexture->Release();
	for( int i = 0; i < DOWNSAMPLE_COUNT; i++ ) {
		downSampSurf[ i ]->Release();
	}
	for( int i = 0; i < DOWNSAMPLE_COUNT; i++ ) {
		downSampTex[ i ]->Release();
	}
	adaptedLuminanceLast->Release();
	adaptedLuminanceCurrent->Release();
}

void HDRShader::OnResetDevice()
{
	int width = Engine::Instance().GetResolutionWidth();
	int height = Engine::Instance().GetResolutionHeight();
	if( !Engine::Instance().IsNonPowerOfTwoTexturesSupport()) {
		width = FloorPow2( Engine::Instance().GetResolutionWidth() );
		height = FloorPow2( Engine::Instance().GetResolutionHeight() );
	}
	D3DXCreateTexture( Engine::Instance().GetDevice(), width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT, &hdrTexture );
	hdrTexture->GetSurfaceLevel( 0, &hdrSurface );

	D3DXCreateTexture( Engine::Instance().GetDevice(), 512, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &scaledScene );
	scaledScene->GetSurfaceLevel( 0, &scaledSceneSurf );

	D3DXCreateTexture( Engine::Instance().GetDevice(), 256, 256, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 0 ]);
	D3DXCreateTexture( Engine::Instance().GetDevice(), 128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 1 ]);
	D3DXCreateTexture( Engine::Instance().GetDevice(), 64, 64, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 2 ]);
	D3DXCreateTexture( Engine::Instance().GetDevice(), 32, 32, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 3 ]);
	D3DXCreateTexture( Engine::Instance().GetDevice(), 16, 16, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 4 ]);
	D3DXCreateTexture( Engine::Instance().GetDevice(), 8, 8, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 5 ]);
	D3DXCreateTexture( Engine::Instance().GetDevice(), 4, 4, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 6 ]);
	D3DXCreateTexture( Engine::Instance().GetDevice(), 2, 2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 7 ]);
	D3DXCreateTexture( Engine::Instance().GetDevice(), 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 8 ]);

	D3DXCreateTexture( Engine::Instance().GetDevice(), 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &adaptedLuminanceLast );
	D3DXCreateTexture( Engine::Instance().GetDevice(), 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &adaptedLuminanceCurrent );

	for( int i = 0; i < DOWNSAMPLE_COUNT; i++ ) {
		downSampTex[i]->GetSurfaceLevel( 0, &downSampSurf[i] );
	}
}
