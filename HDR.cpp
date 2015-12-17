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
#include "Engine.h"
#include "HDR.h"
#include "Utility.h"

void HDRShader::DoToneMapping( IDirect3DSurface9 * renderTarget, IDirect3DTexture9 * hdrFrame ) {
    Engine::I().GetDevice()->SetRenderTarget( 0, renderTarget );
    Engine::I().GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
    Engine::I().GetDevice()->SetTexture( 0, hdrFrame );
    Engine::I().GetDevice()->SetTexture( 7, mAdaptedLuminanceCurrent );
    mToneMapShader->Bind();
    mScreenQuad->Bind();
    mScreenQuad->Render();
}

HDRShader::~HDRShader() {
	OnLostDevice();
}

HDRShader::HDRShader() {
	OnResetDevice();
	
	mToneMapShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/hdrTonemap.pso" ));
	mScreenQuad = unique_ptr<EffectsQuad>( new EffectsQuad( false ));
    mScaleScenePixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/hdrScale.pso" ));
    mAdaptationPixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/hdrAdaptation.pso" ));
	mDownScalePixelShader = unique_ptr<PixelShader>( new PixelShader( "data/shaders/hdrDownscale.pso" ));
}

void HDRShader::CalculateFrameLuminance( IDirect3DTexture9 * hdrFrame ) {
    mScreenQuad->Bind();

    Engine::I().GetDevice()->SetTexture( 7, hdrFrame );
    Engine::I().GetDevice()->SetRenderTarget( 0, mScaledSceneSurf );
    mScaleScenePixelShader->Bind();
    mScreenQuad->Render();

    // next, do downsampling until we get 1x1 luminance texture
    mDownScalePixelShader->Bind();

	Engine::I().GetDevice()->SetTexture( 7, mScaledScene );
	for( int i = 0; i < DOWNSAMPLE_COUNT; i++ ) {
		float pixelSize = 1.0f / static_cast<float>( IntegerPow( 2, DOWNSAMPLE_COUNT - i ));	

		Engine::I().GetDevice()->SetRenderTarget( 0, mDownSampSurf[ i ] );
		Engine::I().SetPixelShaderFloat( 0, pixelSize );

		mScreenQuad->Render();

		Engine::I().GetDevice()->SetTexture( 7, mDownSampTex[ i ] );
	}

	for( int i = 0; i < 8; i++ ) {
		Engine::I().GetDevice()->SetTexture( i, 0 );
	}

    // now we get average frame luminance presented as 1x1 pixel RGBA8 texture
    // render it into R32F luminance texture
    IDirect3DTexture9 * pTexSwap = mAdaptedLuminanceLast;
    mAdaptedLuminanceLast = mAdaptedLuminanceCurrent;
    mAdaptedLuminanceCurrent = pTexSwap;

    IDirect3DSurface9 * pSurfAdaptedLum = NULL;
    mAdaptedLuminanceCurrent->GetSurfaceLevel( 0, &pSurfAdaptedLum );

    Engine::I().GetDevice()->SetRenderTarget( 0, pSurfAdaptedLum );
    Engine::I().GetDevice()->SetTexture( 6, mAdaptedLuminanceLast );
    Engine::I().GetDevice()->SetTexture( 7, mDownSampTex[ DOWNSAMPLE_COUNT - 1 ] );

    mAdaptationPixelShader->Bind();
	Engine::I().SetPixelShaderFloat( 0, 0.75f ); // adaptation
    mScreenQuad->Render();

    pSurfAdaptedLum->Release();
}

void HDRShader::OnLostDevice() {
	mScaledSceneSurf->Release();
	mScaledScene->Release();
	for( int i = 0; i < DOWNSAMPLE_COUNT; i++ ) {
		mDownSampSurf[ i ]->Release();
		mDownSampTex[ i ]->Release();
	}
	mAdaptedLuminanceLast->Release();
	mAdaptedLuminanceCurrent->Release();
}

void HDRShader::OnResetDevice() {
	int scaledSize = IntegerPow( 2, DOWNSAMPLE_COUNT + 1 );
	D3DXCreateTexture( Engine::I().GetDevice(), scaledSize, scaledSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mScaledScene );
	mScaledScene->GetSurfaceLevel( 0, &mScaledSceneSurf );

	for( int i = 0; i < DOWNSAMPLE_COUNT; i++ ) {
		int size = IntegerPow( 2, DOWNSAMPLE_COUNT - i );
		D3DXCreateTexture( Engine::I().GetDevice(), size, size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mDownSampTex[ i ]);
	}

	D3DXCreateTexture( Engine::I().GetDevice(), 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mAdaptedLuminanceLast );
	D3DXCreateTexture( Engine::I().GetDevice(), 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mAdaptedLuminanceCurrent );

	for( int i = 0; i < DOWNSAMPLE_COUNT; i++ ) {
		mDownSampTex[i]->GetSurfaceLevel( 0, &mDownSampSurf[i] );
	}
}
