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



#define USE_R32F_DEPTH

class GBuffer : public RendererComponent {
public:
    IDirect3DTexture9 * depthMap;
    IDirect3DTexture9 * normalMap;
    IDirect3DTexture9 * diffuseMap;

    IDirect3DSurface9 * depthSurface;
    IDirect3DSurface9 * normalSurface;
    IDirect3DSurface9 * diffuseSurface;
    IDirect3DSurface9 * backSurface;

	void CreateRenderTargets();
	void FreeRenderTargets();
public:
    explicit GBuffer();
    virtual ~GBuffer();

	void OnResetDevice();
	void OnLostDevice();
    void BindRenderTargets();
    void UnbindRenderTargets();
    void BindTextures();
    void BindDepthMap( int layer );

    void BindNormalMapAsRT();
    void BindDiffuseMapAsRT();
    void BindDepthMapAsRT();
    void BindBackSurfaceAsRT();
    void UnbindTextures();
};