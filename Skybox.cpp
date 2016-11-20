/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
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

#include "Precompiled.h"
#include "Renderer.h"
#include "Skybox.h"
#include "Texture.h"
#include "Vertex.h"

Skybox::Skybox( shared_ptr<Texture> up, shared_ptr<Texture> left, shared_ptr<Texture> right, shared_ptr<Texture> forward, shared_ptr<Texture> back ) {
	mTextures[ 0 ] = forward;
	mTextures[ 1 ] = back;
	mTextures[ 2 ] = right;
	mTextures[ 3 ] = left;
	mTextures[ 4 ] = up;
}
