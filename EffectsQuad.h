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

#include "Shader.h"

struct QuadVertex {
	float x, y, z;
	float tx, ty;
};

class EffectsQuad : public RendererComponent {
private:
	void Initialize();
	void Free();
	D3DXMATRIX mOrthoProjection;
	COMPtr<IDirect3DVertexBuffer9> mVertexBuffer;
	COMPtr<IDirect3DVertexDeclaration9> mVertexDeclaration;
public:
    bool debug;
    unique_ptr<VertexShader> mVertexShader;
    unique_ptr<PixelShader> mDebugPixelShader; 
    explicit EffectsQuad( bool bDebug = false );
    virtual ~EffectsQuad();
    void SetSize( float width, float height );
    void Bind( bool bindInternalVertexShader = true );
	void BindNoShader();
    void Render();
	void OnResetDevice();
	void OnLostDevice();
};