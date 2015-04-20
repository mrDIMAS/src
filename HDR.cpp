#include "Precompiled.h"

#include "HDR.h"
#include "Utility.h"

void HDRShader::DoToneMapping( IDirect3DSurface9 * targetSurface ) {
    gpDevice->SetRenderTarget( 0, targetSurface );
    gpDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
    gpDevice->SetTexture( 0, hdrTexture );
    gpDevice->SetTexture( 7, adaptedLuminanceCurrent );
    toneMapShader->Bind();
    screenQuad->Bind();
    screenQuad->Render();
}

void HDRShader::SetAsRenderTarget() {
    gpDevice->SetRenderTarget( 0, hdrSurface );
}

HDRShader::~HDRShader() {
    scaledSceneSurf->Release();
    scaledScene->Release();
    hdrSurface->Release();
    hdrTexture->Release();
    delete screenQuad;
    delete toneMapShader;
    delete scaleScenePixelShader;
    delete adaptationPixelShader;
    delete downScalePixelShader;
    for( int i = 0; i < DOWNSAMPLE_COUNT; i++ ) {
        downSampSurf[ i ]->Release();
    }
    for( int i = 0; i < DOWNSAMPLE_COUNT; i++ ) {
        downSampTex[ i ]->Release();
    }
    adaptedLuminanceLast->Release();
    adaptedLuminanceCurrent->Release();
}

HDRShader::HDRShader( D3DFORMAT rtFormat ) {
	int width = g_width;
	int height = g_height;
	if( !IsFullNPOTTexturesSupport()) {
		width = NearestPow2( g_width );
		height = NearestPow2( g_height );
	}
    D3DXCreateTexture( gpDevice, width, height, 0, D3DUSAGE_RENDERTARGET  , rtFormat, D3DPOOL_DEFAULT, &hdrTexture );
    hdrTexture->GetSurfaceLevel( 0, &hdrSurface );

    string toneMapShaderSource =

		"sampler hdrTexture : register( s0 );\n"
		"sampler avgLum : register( s7 );\n"
		"float4 main( float2 texCoord : TEXCOORD0 ) : COLOR0{\n"
		"	float luminance = clamp( tex2D( avgLum, float2( 0.5f, 0.5f )), 0.041, 1.5f ).r;\n"
		"	float3 texColor = tex2D( hdrTexture, texCoord );\n"
		"	texColor /= luminance * 10;\n"
		"	return float4( texColor, 1 );\n"
		"};\n";

    toneMapShader = new PixelShader( toneMapShaderSource );
    screenQuad = new EffectsQuad( false );


    D3DXCreateTexture( gpDevice, 512, 512, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &scaledScene );
    scaledScene->GetSurfaceLevel( 0, &scaledSceneSurf );

    D3DXCreateTexture( gpDevice, 256, 256, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 0 ]);
    D3DXCreateTexture( gpDevice, 128, 128, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 1 ]);
    D3DXCreateTexture( gpDevice, 64, 64, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 2 ]);
    D3DXCreateTexture( gpDevice, 32, 32, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 3 ]);
    D3DXCreateTexture( gpDevice, 16, 16, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 4 ]);
    D3DXCreateTexture( gpDevice, 8, 8, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 5 ]);
    D3DXCreateTexture( gpDevice, 4, 4, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 6 ]);
    D3DXCreateTexture( gpDevice, 2, 2, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 7 ]);
    D3DXCreateTexture( gpDevice, 1, 1, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 8 ]);

    D3DXCreateTexture( gpDevice, 1, 1, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &adaptedLuminanceLast );
    D3DXCreateTexture( gpDevice, 1, 1, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &adaptedLuminanceCurrent );


    string scalePixelShaderSource =
        "sampler hdrTexture : register( s7 );\n"
        "float4 main( float2 texCoord : TEXCOORD0 ) : COLOR0 {\n"
        "	float4 hdrTexel = tex2D( hdrTexture, texCoord );\n"
        //"	return hdrTexel.r * 0.27 + hdrTexel.g * 0.67 + hdrTexel.b * 0.06;\n"
		"	return hdrTexel;\n"
        "};\n";

    scaleScenePixelShader = new PixelShader( scalePixelShaderSource );

    string adaptationPixelShaderSource =
        "sampler last : register( s6 );\n"
        "sampler current : register( s7 );\n"
        "float adaptation;\n"
        "float4 main( ) : COLOR0\n {"
        "	float fAdaptedLum = tex2D( last, float2(0.5f, 0.5f)).r;\n"
        "	float fCurrentLum = tex2D( current, float2(0.5f, 0.5f)).r;\n"
		"	float fNewAdaptation = fAdaptedLum + (fCurrentLum - fAdaptedLum) * ( 1 - pow( 0.98, adaptation ));\n"
        "	return float4(fNewAdaptation, 0.0f, 0.0f, 1.0f);\n"
        "};\n";

    adaptationPixelShader = new PixelShader( adaptationPixelShaderSource );
    hAdaptation = adaptationPixelShader->GetConstantTable()->GetConstantByName( 0, "adaptation" );

    for( int i = 0; i < DOWNSAMPLE_COUNT; i++ ) {
        downSampTex[i]->GetSurfaceLevel( 0, &downSampSurf[i] );
    }

    string downScale2x2 =
        "sampler s0 : register( s7 );\n"
        "float pixelSize;\n"
        "float4 main( float2 texCoord : TEXCOORD0 ) : COLOR0 {\n"
        "	float sample = 0.0f;\n"
        "	sample += tex2D( s0, texCoord + float2( -pixelSize, -pixelSize ) ).r;\n"
        "	sample += tex2D( s0, texCoord + float2(  pixelSize, -pixelSize ) ).r;\n"
        "	sample += tex2D( s0, texCoord + float2(  pixelSize,  pixelSize ) ).r;\n"
        "	sample += tex2D( s0, texCoord + float2( -pixelSize,  pixelSize ) ).r;\n"
        "	return sample / 4;\n"
        "}\n";

    downScalePixelShader = new PixelShader( downScale2x2 );
    hPixelSize = downScalePixelShader->GetConstantTable()->GetConstantByName( 0, "pixelSize" );
}

void HDRShader::CalculateFrameLuminance( ) {
    screenQuad->Bind();

    gpDevice->SetTexture( 7, hdrTexture );
    gpDevice->SetRenderTarget( 0, scaledSceneSurf );
    scaleScenePixelShader->Bind();
    screenQuad->Render();

    // next, do downsampling until we get 1x1 luminance texture
    downScalePixelShader->Bind();
    // 256x256
    gpDevice->SetTexture( 7, scaledScene );
    gpDevice->SetRenderTarget( 0, downSampSurf[ 0 ] );
    downScalePixelShader->GetConstantTable()->SetFloat( gpDevice, hPixelSize, 1.0f / 256.0f );
    screenQuad->Render();
    // 128x128
    gpDevice->SetTexture( 7, downSampTex[ 0 ] );
    gpDevice->SetRenderTarget( 0, downSampSurf[ 1 ] );
    downScalePixelShader->GetConstantTable()->SetFloat( gpDevice, hPixelSize, 1.0f / 128.0f );
    screenQuad->Render();
    // 64x64
    gpDevice->SetTexture( 7, downSampTex[ 1 ] );
    gpDevice->SetRenderTarget( 0, downSampSurf[ 2 ] );
    downScalePixelShader->GetConstantTable()->SetFloat( gpDevice, hPixelSize, 1.0f / 64.0f );
    screenQuad->Render();
    // 32x32
    gpDevice->SetTexture( 7, downSampTex[ 2 ] );
    gpDevice->SetRenderTarget( 0, downSampSurf[ 3 ] );
    downScalePixelShader->GetConstantTable()->SetFloat( gpDevice, hPixelSize, 1.0f / 32.0f );
    screenQuad->Render();
    // 16x16
    gpDevice->SetTexture( 7, downSampTex[ 3 ] );
    gpDevice->SetRenderTarget( 0, downSampSurf[ 4 ] );
    downScalePixelShader->GetConstantTable()->SetFloat( gpDevice, hPixelSize, 1.0f / 16.0f );
    screenQuad->Render();
    // 8x8
    gpDevice->SetTexture( 7, downSampTex[ 4 ] );
    gpDevice->SetRenderTarget( 0, downSampSurf[ 5 ] );
    downScalePixelShader->GetConstantTable()->SetFloat( gpDevice, hPixelSize, 1.0f / 8.0f );
    screenQuad->Render();
    // 4x4
    gpDevice->SetTexture( 7, downSampTex[ 5 ] );
    gpDevice->SetRenderTarget( 0, downSampSurf[ 6 ] );
    downScalePixelShader->GetConstantTable()->SetFloat( gpDevice, hPixelSize, 1.0f / 4.0f );
    screenQuad->Render();
    // 2x2
    gpDevice->SetTexture( 7, downSampTex[ 6 ] );
    gpDevice->SetRenderTarget( 0, downSampSurf[ 7 ] );
    downScalePixelShader->GetConstantTable()->SetFloat( gpDevice, hPixelSize, 1.0f / 2.0f );
    screenQuad->Render();
    // final 1x1
    gpDevice->SetTexture( 7, downSampTex[ 7 ] );
    gpDevice->SetRenderTarget( 0, downSampSurf[ 8 ] );
    downScalePixelShader->GetConstantTable()->SetFloat( gpDevice, hPixelSize, 1.0f );
    screenQuad->Render();
    // now we get average frame luminance presented as 1x1 pixel RGBA8 texture
    // render it into R32F luminance texture
    for( int i = 0; i < 8; i++ ) {
        gpDevice->SetTexture( i, 0 );
    }

    IDirect3DTexture9 * pTexSwap = adaptedLuminanceLast;
    adaptedLuminanceLast = adaptedLuminanceCurrent;
    adaptedLuminanceCurrent = pTexSwap;

    IDirect3DSurface9 * pSurfAdaptedLum = NULL;
    adaptedLuminanceCurrent->GetSurfaceLevel( 0, &pSurfAdaptedLum );

    gpDevice->SetRenderTarget( 0, pSurfAdaptedLum );
    gpDevice->SetTexture( 6, adaptedLuminanceLast );
    gpDevice->SetTexture( 7, downSampTex[8] );

    adaptationPixelShader->Bind();
    adaptationPixelShader->GetConstantTable()->SetFloat( gpDevice, hAdaptation, 0.75f );
    screenQuad->Render();

    pSurfAdaptedLum->Release();
}
