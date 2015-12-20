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

#include "DeferredRenderer.h"

class MultipleRTDeferredRenderer : public DeferredRenderer {
private:
	// Standard GBuffer shader
	unique_ptr<VertexShader> mVertexShader;
	unique_ptr<PixelShader> mPixelShader;

	// Standard GBuffer shader with skinning
	unique_ptr<VertexShader> mVertexShaderSkin;
	unique_ptr<PixelShader> mPixelShaderSkin;

	// Parallax occlusion mapping shaders
	unique_ptr<VertexShader> mVertexShaderPOM;
	unique_ptr<PixelShader> mPixelShaderPOM;	
public:
    explicit MultipleRTDeferredRenderer( );
    virtual ~MultipleRTDeferredRenderer();
	virtual void BindParallaxShaders();
	virtual void BindGenericShaders();
	virtual void BindGenericSkinShaders();
    void BeginFirstPass();
    void RenderMesh( shared_ptr<Mesh> mesh );
    void OnEnd();
};