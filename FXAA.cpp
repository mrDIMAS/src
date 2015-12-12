/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/

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
	OnResetDevice();
    effectsQuad = new EffectsQuad;
}

FXAA::~FXAA() {
    delete pixelShader;
    delete effectsQuad;
    OnLostDevice();
}

void FXAA::OnLostDevice()
{
	backBufferRT->Release();
	renderTarget->Release();
	texture->Release();
}

void FXAA::OnResetDevice()
{
	int width = Engine::Instance().GetResolutionWidth();
	int height = Engine::Instance().GetResolutionHeight();
	if( !Engine::Instance().IsNonPowerOfTwoTexturesSupport()) {
		width = FloorPow2( Engine::Instance().GetResolutionWidth() );
		height = FloorPow2( Engine::Instance().GetResolutionHeight() );
	}
	if( FAILED( D3DXCreateTexture( Engine::Instance().GetDevice(), width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture ))) {
		MessageBoxA( 0, "Failed to create FXAA texture.", 0, MB_OK | MB_ICONERROR );
	}
	texture->GetSurfaceLevel( 0, &renderTarget );
	backBufferRT = Engine::Instance().GetBackBuffer();
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