#include "HDR.h"
#include "Utility.h"

void HDRRenderer::DoToneMapping( IDirect3DSurface9 * targetSurface )
{
    g_pDevice->SetRenderTarget( 0, targetSurface );
    g_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );
    g_pDevice->SetTexture( 0, hdrTexture );
    g_pDevice->SetTexture( 7, adaptedLuminanceCurrent );
    toneMapShader->Bind();
    screenQuad->Bind();
    g_pDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
    g_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
    g_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
    g_pDevice->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_NOTEQUAL);
    g_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );
    g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    g_pDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
    g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    screenQuad->Render();
}

void HDRRenderer::SetAsRenderTarget()
{
    g_pDevice->SetRenderTarget( 0, hdrSurface );
}

HDRRenderer::~HDRRenderer()
{
    scaledSceneSurf->Release();
    scaledScene->Release();
    hdrSurface->Release();
    hdrTexture->Release();
    delete screenQuad;
    delete toneMapShader;
    delete scaleScenePixelShader;
    delete adaptationPixelShader;
    delete downScalePixelShader;
    for( int i = 0; i < DOWNSAMPLE_COUNT; i++ )
        downSampSurf[ i ]->Release();
    for( int i = 0; i < DOWNSAMPLE_COUNT; i++ )
        downSampTex[ i ]->Release();
    adaptedLuminanceLast->Release();
    adaptedLuminanceCurrent->Release();
}

HDRRenderer::HDRRenderer( D3DFORMAT rtFormat )
{
    D3DXCreateTexture( g_pDevice, g_width, g_height, 0, D3DUSAGE_RENDERTARGET  , rtFormat, D3DPOOL_DEFAULT, &hdrTexture );
    hdrTexture->GetSurfaceLevel( 0, &hdrSurface );

    string toneMapShaderSource =
        "sampler hdrTexture : register( s0 );\n"
        "sampler avgLum : register( s7 );\n"

        "float4 main( float2 texCoord : TEXCOORD0 ) : COLOR0{\n"
        "	float exposure = tex2D( avgLum, float2( 0.5f, 0.5f )).r;\n"
        "	float3 texColor = tex2D( hdrTexture, texCoord );\n"
        "	float key = 0.1f;\n"
        "	texColor *=  key / ( exposure + 0.034f );\n"
        "	texColor /= ( 1.0f + texColor );\n"
        "	return float4(texColor,1);\n"
        "};\n";

    toneMapShader = new PixelShader( toneMapShaderSource );
    screenQuad = new EffectsQuad( false );


    D3DXCreateTexture( g_pDevice, 512, 512, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &scaledScene );
    scaledScene->GetSurfaceLevel( 0, &scaledSceneSurf );

    D3DXCreateTexture( g_pDevice, 256, 256, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 0 ]);
    D3DXCreateTexture( g_pDevice, 128, 128, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 1 ]);
    D3DXCreateTexture( g_pDevice, 64, 64, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 2 ]);
    D3DXCreateTexture( g_pDevice, 32, 32, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 3 ]);
    D3DXCreateTexture( g_pDevice, 16, 16, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 4 ]);
    D3DXCreateTexture( g_pDevice, 8, 8, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 5 ]);
    D3DXCreateTexture( g_pDevice, 4, 4, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 6 ]);
    D3DXCreateTexture( g_pDevice, 2, 2, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 7 ]);
    D3DXCreateTexture( g_pDevice, 1, 1, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &downSampTex[ 8 ]);

    D3DXCreateTexture( g_pDevice, 1, 1, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &adaptedLuminanceLast );
    D3DXCreateTexture( g_pDevice, 1, 1, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &adaptedLuminanceCurrent );


    string scalePixelShaderSource =
        "sampler hdrTexture : register( s7 );\n"
        "float4 main( float2 texCoord : TEXCOORD0 ) : COLOR0 {\n"
        "	float4 hdrTexel = tex2D( hdrTexture, texCoord );\n"
        "	return hdrTexel.r * 0.27 + hdrTexel.g * 0.67 + hdrTexel.b * 0.06;\n"
        "};\n";

    scaleScenePixelShader = new PixelShader( scalePixelShaderSource );

    string adaptationPixelShaderSource =
        "sampler s0 : register( s6 );\n"
        "sampler s1 : register( s7 );\n"
        "float adaptation;\n"
        "float4 main( ) : COLOR0\n {"
        "	float fAdaptedLum = tex2D(s0, float2(0.5f, 0.5f)).r;\n"
        "	float fCurrentLum = tex2D(s1, float2(0.5f, 0.5f)).r;\n"
        "	float fNewAdaptation = fAdaptedLum + (fCurrentLum - fAdaptedLum) * ( 1 - pow( 0.98f, adaptation ) );\n"
        "	return float4(fNewAdaptation, fNewAdaptation, fNewAdaptation, 1.0f);\n"
        "};\n";

    adaptationPixelShader = new PixelShader( adaptationPixelShaderSource );
    hAdaptation = adaptationPixelShader->GetConstantTable()->GetConstantByName( 0, "adaptation" );

    for( int i = 0; i < DOWNSAMPLE_COUNT; i++ )
        downSampTex[i]->GetSurfaceLevel( 0, &downSampSurf[i] );

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

void HDRRenderer::CalculateFrameLuminance( )
{
    g_pDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
    g_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
    g_pDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
    g_pDevice->SetRenderState( D3DRS_CCW_STENCILFUNC, D3DCMP_NOTEQUAL );
    g_pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );
    g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    g_pDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
    g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    screenQuad->Bind();

    g_pDevice->SetTexture( 7, hdrTexture );
    g_pDevice->SetRenderTarget( 0, scaledSceneSurf );
    scaleScenePixelShader->Bind();
    screenQuad->Render();

    // next, do downsampling until we get 1x1 luminance texture
    downScalePixelShader->Bind();
    // 256x256
    g_pDevice->SetTexture( 7, scaledScene );
    g_pDevice->SetRenderTarget( 0, downSampSurf[ 0 ] );
    //downScaleQuad->SetSize( 256, 256 );
    downScalePixelShader->GetConstantTable()->SetFloat( g_pDevice, hPixelSize, 1.0f / 256.0f );
    screenQuad->Render();
    // 128x128
    g_pDevice->SetTexture( 7, downSampTex[ 0 ] );
    g_pDevice->SetRenderTarget( 0, downSampSurf[ 1 ] );
    //downScaleQuad->SetSize( 128, 128 );
    downScalePixelShader->GetConstantTable()->SetFloat( g_pDevice, hPixelSize, 1.0f / 128.0f );
    screenQuad->Render();
    // 64x64
    g_pDevice->SetTexture( 7, downSampTex[ 1 ] );
    g_pDevice->SetRenderTarget( 0, downSampSurf[ 2 ] );
    //downScaleQuad->SetSize( 64, 64 );
    downScalePixelShader->GetConstantTable()->SetFloat( g_pDevice, hPixelSize, 1.0f / 64.0f );
    screenQuad->Render();
    // 32x32
    g_pDevice->SetTexture( 7, downSampTex[ 2 ] );
    g_pDevice->SetRenderTarget( 0, downSampSurf[ 3 ] );
    //downScaleQuad->SetSize( 32, 32 );
    downScalePixelShader->GetConstantTable()->SetFloat( g_pDevice, hPixelSize, 1.0f / 32.0f );
    screenQuad->Render();
    // 16x16
    g_pDevice->SetTexture( 7, downSampTex[ 3 ] );
    g_pDevice->SetRenderTarget( 0, downSampSurf[ 4 ] );
    //downScaleQuad->SetSize( 16, 16 );
    downScalePixelShader->GetConstantTable()->SetFloat( g_pDevice, hPixelSize, 1.0f / 16.0f );
    screenQuad->Render();
    // 8x8
    g_pDevice->SetTexture( 7, downSampTex[ 4 ] );
    g_pDevice->SetRenderTarget( 0, downSampSurf[ 5 ] );
    //downScaleQuad->SetSize( 8, 8 );
    downScalePixelShader->GetConstantTable()->SetFloat( g_pDevice, hPixelSize, 1.0f / 8.0f );
    screenQuad->Render();
    // 4x4
    g_pDevice->SetTexture( 7, downSampTex[ 5 ] );
    g_pDevice->SetRenderTarget( 0, downSampSurf[ 6 ] );
    //downScaleQuad->SetSize( 4, 4 );
    downScalePixelShader->GetConstantTable()->SetFloat( g_pDevice, hPixelSize, 1.0f / 4.0f );
    screenQuad->Render();
    // 2x2
    g_pDevice->SetTexture( 7, downSampTex[ 6 ] );
    g_pDevice->SetRenderTarget( 0, downSampSurf[ 7 ] );
    //downScaleQuad->SetSize( 2, 2 );
    downScalePixelShader->GetConstantTable()->SetFloat( g_pDevice, hPixelSize, 1.0f / 2.0f );
    screenQuad->Render();
    // final 1x1
    g_pDevice->SetTexture( 7, downSampTex[ 7 ] );
    g_pDevice->SetRenderTarget( 0, downSampSurf[ 8 ] );
    //downScaleQuad->SetSize( 1, 1 );
    downScalePixelShader->GetConstantTable()->SetFloat( g_pDevice, hPixelSize, 1.0f );
    screenQuad->Render();
    // now we get average frame luminance presented as 1x1 pixel RGBA8 texture
    // render it into R32F luminance texture
    for( int i = 0; i < 8; i++ )
        g_pDevice->SetTexture( i, 0 );

    PDIRECT3DTEXTURE9 pTexSwap = adaptedLuminanceLast;
    adaptedLuminanceLast = adaptedLuminanceCurrent;
    adaptedLuminanceCurrent = pTexSwap;

    PDIRECT3DSURFACE9 pSurfAdaptedLum = NULL;
    adaptedLuminanceCurrent->GetSurfaceLevel( 0, &pSurfAdaptedLum );

    g_pDevice->SetRenderTarget( 0, pSurfAdaptedLum );
    g_pDevice->SetTexture( 6, adaptedLuminanceLast );
    g_pDevice->SetTexture( 7, downSampTex[8] );

    adaptationPixelShader->Bind();
    adaptationPixelShader->GetConstantTable()->SetFloat( g_pDevice, hAdaptation, 0.2f );
    screenQuad->Render();

    pSurfAdaptedLum->Release();
}
