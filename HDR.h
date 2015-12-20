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

#pragma once


#include "EffectsQuad.h"

#define DOWNSAMPLE_COUNT 6

class HDRShader : public RendererComponent {
private:
    unique_ptr<EffectsQuad> mScreenQuad;
    unique_ptr<PixelShader> mToneMapShader;
    unique_ptr<PixelShader> mDownScalePixelShader;
    unique_ptr<PixelShader> mAdaptationPixelShader;
    unique_ptr<PixelShader> mScaleScenePixelShader;

    COMPtr<IDirect3DTexture9> mScaledScene;
    COMPtr<IDirect3DSurface9> mScaledSceneSurf;
    COMPtr<IDirect3DTexture9> mDownSampTex[ DOWNSAMPLE_COUNT ];
    COMPtr<IDirect3DSurface9> mDownSampSurf[ DOWNSAMPLE_COUNT ];
    COMPtr<IDirect3DTexture9> mAdaptedLuminanceLast;
    COMPtr<IDirect3DTexture9> mAdaptedLuminanceCurrent;
public:
    HDRShader();
    ~HDRShader();
    void CalculateFrameLuminance( IDirect3DTexture9 * hdrFrame );
    void DoToneMapping( IDirect3DSurface9 * renderTarget, IDirect3DTexture9 * hdrFrame );
	void OnResetDevice();
	void OnLostDevice();
};