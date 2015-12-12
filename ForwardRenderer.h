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


#include "Light.h"
#include "Camera.h"
#include "Mesh.h"
#include "Utility.h"
#include "Shader.h"

// must be used to render transparent faces only
// in other cases prefer DeferredRenderer
class ForwardRenderer {
private:
    map< IDirect3DTexture9*, vector< Mesh* > > mRenderList;
    shared_ptr<PixelShader> mPixelShader;
    shared_ptr<VertexShader> mVertexShader;
public:
    ForwardRenderer();
    ~ForwardRenderer();
    void AddMesh( Mesh * mesh );
    void RemoveMesh( Mesh * mesh );
    void RenderMeshes();
};