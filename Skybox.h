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



class Skybox : public RendererComponent {
private:
	struct SkyVertex {
		float x, y, z;
		float tx, ty;
	};
	Texture * mTextures[5];
	IDirect3DVertexBuffer9 * mVertexBuffer;
	IDirect3DIndexBuffer9 * mIndexBuffer;
	IDirect3DVertexDeclaration9 * mVertexDeclaration;
public:
    explicit Skybox( Texture * up, Texture * left, Texture * right, Texture * forward, Texture * back );
    virtual ~Skybox();
    void Render( );
	void OnResetDevice();
	void OnLostDevice();
};
