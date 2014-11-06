#pragma once

#include "Common.h"
#include "EffectsQuad.h"

class HDRRenderer {
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
	D3DXHANDLE hPixelSize;
	D3DXHANDLE hAdaptation;
public:
	explicit HDRRenderer( D3DFORMAT rtFormat );
	virtual ~HDRRenderer();
	void SetAsRenderTarget( );
	void CalculateFrameLuminance( );
	void DoToneMapping( IDirect3DSurface9 * targetSurface );
};