#include "FXAA.h"
#include "Renderer.h"

void FXAA::DoAntialiasing( IDirect3DTexture9 * outTexture ) {
    gpDevice->SetRenderTarget( 0, backBufferRT );
    gpDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

    gpDevice->SetTexture( 0, outTexture );

    effectsQuad->Bind();
    pixelShader->Bind();

	float screenMetrics[] = { g_width, g_height, 0.0f };
	gpRenderer->SetPixelShaderFloat3( 0, screenMetrics );
    effectsQuad->Render();
}

void FXAA::BeginDrawIntoTexture() {
    gpDevice->SetRenderTarget( 0, renderTarget );
}

FXAA::FXAA() {
	pixelShader = new PixelShader( "data/shaders/fxaa.pso", true );
	int width = g_width;
	int height = g_height;
	if( !IsFullNPOTTexturesSupport()) {
		width = NearestPow2( g_width );
		height = NearestPow2( g_height );
	}
    if( FAILED( D3DXCreateTexture( gpDevice, width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture ))) {
        MessageBoxA( 0, "Failed to create FXAA texture.", 0, MB_OK | MB_ICONERROR );
    }
    texture->GetSurfaceLevel( 0, &renderTarget );
    gpDevice->GetRenderTarget( 0, &backBufferRT );
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

RUAPI void ruEnableFXAA( ) {
    g_fxaaEnabled = true;
}

RUAPI void ruDisableFXAA( ) {
    g_fxaaEnabled = false;
}

RUAPI bool ruFXAAEnabled() {
    return g_fxaaEnabled;
}