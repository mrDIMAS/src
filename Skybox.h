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
	shared_ptr<Texture> mTextures[5];
	COMPtr<IDirect3DVertexBuffer9> mVertexBuffer;
	COMPtr<IDirect3DIndexBuffer9> mIndexBuffer;
	COMPtr<IDirect3DVertexDeclaration9> mVertexDeclaration;
public:
    explicit Skybox( shared_ptr<Texture> up, shared_ptr<Texture> left, shared_ptr<Texture> right, shared_ptr<Texture> forward, shared_ptr<Texture> back );
    virtual ~Skybox();
    void Render( );
	void OnResetDevice();
	void OnLostDevice();
};
