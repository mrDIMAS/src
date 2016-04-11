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
#include "BitmapFont.h"
#include "GUIRect.h"
#include "GUIText.h"
#include "GUIButton.h"

class Face {
private:
	unsigned short mIndex[6];
public:    
	void Set( unsigned short i1, unsigned short i2, unsigned short i3, unsigned short i4, unsigned short i5, unsigned short i6 );
};

class GUIRenderer : public RendererComponent {
private:
    COMPtr<IDirect3DVertexBuffer9> mVertexBuffer;
    int mSizeOfRectBytes;
    D3DXMATRIX mOrthoMatrix;
    void RenderRect( const shared_ptr<GUIRect> & r );
    unique_ptr<VertexShader> mVertexShader;
    unique_ptr<PixelShader> mPixelShader;
	void Initialize();
public:
    GUIRenderer();
    ~GUIRenderer();
	void OnResetDevice();
	void OnLostDevice();
    //ruFontHandle CreateFont( int size, const string & name );
    void Render();
};