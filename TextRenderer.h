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


#include "BitmapFont.h"
#include "Shader.h"

class GUIText;

class TextRenderer : public RendererComponent {
private:
    int mMaxChars;
    COMPtr<IDirect3DVertexBuffer9> mVertexBuffer;
    COMPtr<IDirect3DIndexBuffer9> mIndexBuffer;
public:
    explicit TextRenderer( );
    virtual ~TextRenderer( );
    void ComputeTextMetrics( const shared_ptr<GUIText> & guiText, int & lines, int & height, int & avWidth, int & avSymbolWidth );
    void RenderText( const shared_ptr<GUIText> & guiText );
	void OnResetDevice();
	void OnLostDevice();
};