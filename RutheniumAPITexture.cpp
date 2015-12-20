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
#include "Precompiled.h"
#include "Texture.h"
#include "CubeTexture.h"

ruTextureHandle ruTextureHandle::Empty() {
	ruTextureHandle handle;
	handle.pointer = 0;

	return handle;
}

ruTextureHandle ruTextureHandle::Request( const string & file ) {
	ruTextureHandle handle;
	handle.pointer = Texture::Request( file );
	return handle;
}

string ruTextureHandle::GetName() {
	return ((Texture*)pointer)->GetName();
}

int ruTextureHandle::GetWidth() {
	return ((Texture*)pointer)->GetWidth();
}

int ruTextureHandle::GetHeight() {
	return ((Texture*)pointer)->GetHeight();
}

ruCubeTextureHandle ruCubeTextureHandle::Request( const string & file ) {
	ruCubeTextureHandle handle;
	handle.pointer = CubeTexture::Request( file );
	return handle;
}