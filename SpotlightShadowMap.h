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


#include "SpotLight.h"
#include "Shader.h"
#include "Mesh.h"
#include "Utility.h"

class SpotlightShadowMap : public RendererComponent {
private:
    IDirect3DTexture9 * spotShadowMap;
    IDirect3DSurface9 * spotSurface;
    IDirect3DSurface9 * defaultDepthStencil;
    IDirect3DSurface9 * depthStencil;
    PixelShader * pixelShader;
    VertexShader * vertexShader;
	void Initialize();
public:
	void OnResetDevice();
	void OnLostDevice();
    int iSize;
    explicit SpotlightShadowMap( float size = 1024.0f );
    virtual ~SpotlightShadowMap();
    void RenderSpotShadowMap( IDirect3DSurface9 * lastUsedRT, int rtIndex, const shared_ptr<SpotLight> & spotLight );
    void BindSpotShadowMap( int index );
    void UnbindSpotShadowMap( int index );
};