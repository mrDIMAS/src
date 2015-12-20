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

class Shader : public RendererComponent {
protected:
	string mSourceName;
	DWORD * LoadBinary();
public:
	explicit Shader( const string & sourceName );
	virtual ~Shader();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

class VertexShader : public Shader {
private:
    COMPtr<IDirect3DVertexShader9> shader;
	static IDirect3DVertexShader9 * msLastBinded;
	virtual void Initialize();
public:
    explicit VertexShader( string fileName );
    virtual ~VertexShader();
    void Bind();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

class PixelShader : public Shader {
private:
    COMPtr<IDirect3DPixelShader9> shader;
	static IDirect3DPixelShader9 * msLastBinded;
	virtual void Initialize();
public:
    explicit PixelShader( string fileName );
    virtual ~PixelShader();
    void Bind();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

