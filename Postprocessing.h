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

#pragma  once

#include "Shader.h"
#include "EffectsQuad.h"

class Postprocessing : public RendererComponent {
private:
	unique_ptr<PixelShader> mMaskPixelShader;
	unique_ptr<PixelShader> mBlurShader;
	unique_ptr<VertexShader> mBlurVertexShader;
	unique_ptr<VertexShader> mBlurSkinVertexShader;
	unique_ptr<EffectsQuad> mEffectsQuad;
	COMPtr<IDirect3DTexture9> mBlurMask;
	COMPtr<IDirect3DSurface9> mBlurMaskSurface;	
public:
	explicit Postprocessing( );
	~Postprocessing( );
	void RenderMask();
	void DoPostprocessing( IDirect3DSurface9 * renderTarget, IDirect3DTexture9 * frameTexture );

	void OnResetDevice();
	void OnLostDevice();
};
