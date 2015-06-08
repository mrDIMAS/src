#pragma once


#include "EffectsQuad.h"

class HDRShader : public RendererComponent {
public:
    IDirect3DTexture9 * hdrTexture;
    IDirect3DSurface9 * hdrSurface;

    EffectsQuad * screenQuad;
    PixelShader * toneMapShader;

    enum {
        DOWNSAMPLE_256X256,
        DOWNSAMPLE_128X128,
        DOWNSAMPLE_64X64,
        DOWNSAMPLE_32X32,
        DOWNSAMPLE_16X16,
        DOWNSAMPLE_8X8,
        DOWNSAMPLE_4X4,
        DOWNSAMPLE_2X2,
        DOWNSAMPLE_1X1, // frame luminance
        DOWNSAMPLE_COUNT,
    };
    PixelShader * downScalePixelShader;
    PixelShader * adaptationPixelShader;
    PixelShader * scaleScenePixelShader;
    IDirect3DTexture9 * scaledScene;
    IDirect3DSurface9 * scaledSceneSurf;
    IDirect3DTexture9 * downSampTex[ DOWNSAMPLE_COUNT ];
    IDirect3DSurface9 * downSampSurf[ DOWNSAMPLE_COUNT ];
    IDirect3DTexture9 * adaptedLuminanceLast;
    IDirect3DTexture9 * adaptedLuminanceCurrent;

public:
    explicit HDRShader();
    virtual ~HDRShader();
    void SetAsRenderTarget( );
    void CalculateFrameLuminance( );
    void DoToneMapping( IDirect3DSurface9 * targetSurface );
	void OnResetDevice();
	void OnLostDevice();
};