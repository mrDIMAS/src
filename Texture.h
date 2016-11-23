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

#pragma once

class Texture : public ITexture {
private:
	COMPtr<IDirect3DTexture9> mTexture;
	int mHeight;
	int mWidth;
	int mColorDepth;
	string mName;
public:
	explicit Texture(const string & name);
	virtual ~Texture();
	bool LoadFromFile(const string & file);
	IDirect3DTexture9 * GetInterface();

	virtual int GetHeight( ) const override final;
	virtual int GetWidth( ) const override final;
	virtual int GetColorDepth( ) const override final;
	virtual string GetName( ) const override final;
};