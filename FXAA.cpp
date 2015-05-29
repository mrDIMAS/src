#include "Precompiled.h"
#include "Utility.h"
#include "FXAA.h"
#include "Engine.h"

void FXAA::DoAntialiasing( IDirect3DTexture9 * outTexture ) {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, backBufferRT );
    Engine::Instance().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

    Engine::Instance().GetDevice()->SetTexture( 0, outTexture );

    effectsQuad->Bind();
    pixelShader->Bind();

	float screenMetrics[] = { Engine::Instance().GetResolutionWidth(), Engine::Instance().GetResolutionHeight(), 0.0f };
	Engine::Instance().SetPixelShaderFloat3( 0, screenMetrics );
    effectsQuad->Render();
}

void FXAA::BeginDrawIntoTexture() {
    Engine::Instance().GetDevice()->SetRenderTarget( 0, renderTarget );
}

FXAA::FXAA() {
	pixelShader = new PixelShader( "data/shaders/fxaa.pso" );
	int width = Engine::Instance().GetResolutionWidth();
	int height = Engine::Instance().GetResolutionHeight();
	if( !Engine::Instance().IsNonPowerOfTwoTexturesSupport()) {
		width = NearestPow2( Engine::Instance().GetResolutionWidth() );
		height = NearestPow2( Engine::Instance().GetResolutionHeight() );
	}
    if( FAILED( D3DXCreateTexture( Engine::Instance().GetDevice(), width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture ))) {
        MessageBoxA( 0, "Failed to create FXAA texture.", 0, MB_OK | MB_ICONERROR );
    }
    texture->GetSurfaceLevel( 0, &renderTarget );
    Engine::Instance().GetDevice()->GetRenderTarget( 0, &backBufferRT );
    effectsQuad = new EffectsQuad;
}

FXAA::~FXAA() {
    delete pixelShader;
    delete effectsQuad;
    renderTarget->Release();
    texture->Release();
}

//////////////////////////////////////////////////////////////////////////
// API
//////////////////////////////////////////////////////////////////////////

void ruEnableFXAA( ) {
    Engine::Instance().SetFXAAEnabled( true );
}

void ruDisableFXAA( ) {
    Engine::Instance().SetFXAAEnabled( false );
}

bool ruFXAAEnabled() {
    return Engine::Instance().IsFXAAEnabled();
}