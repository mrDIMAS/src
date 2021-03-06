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

class FastReader {
private:
	long mSize;
	long mCursor;
	typedef unsigned char Byte;
	Byte * mData;
public:
	FastReader( );
	bool ReadFile( const string & fileName );
	~FastReader( );
	int GetInteger( );
	unsigned short GetShort( );
	unsigned char GetByte( );
	float GetFloat( );
	string GetString( );
	btVector3 GetVector( );
	Vector3 GetBareVector( );
	Vector2 GetBareVector2( );
	btQuaternion GetQuaternion( );
	bool IsEnded( );
};

