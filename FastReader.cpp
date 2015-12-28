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

#include "FastReader.h"

bool FastReader::IsEnded() {
    return mCursor > mSize;
}

btQuaternion FastReader::GetQuaternion() {
    float x = GetFloat();
    float y = GetFloat();
    float z = GetFloat();
    float w = GetFloat();

    return btQuaternion( x, y, z, w );
}

ruVector2 FastReader::GetBareVector2() {
    float x = GetFloat();
    float y = GetFloat();

    return ruVector2( x, y );
}

ruVector3 FastReader::GetBareVector() {
    float x = GetFloat();
    float y = GetFloat();
    float z = GetFloat();

    return ruVector3( x, y, z );
}

btVector3 FastReader::GetVector() {
    float x = GetFloat();
    float y = GetFloat();
    float z = GetFloat();

    return btVector3( x, y, z );
}

std::string FastReader::GetString() {
    string out;

    for ( ; mData[ mCursor ] != '\0'; ++mCursor ) {
        out += mData[ mCursor ];
    }

    if ( mData[ mCursor ] == '\0' ) {
        ++mCursor;
    }

    return out;
}

float FastReader::GetFloat() {
    float value = *reinterpret_cast<float*>( mData + mCursor );

    mCursor += sizeof(float);

    return value;
}

unsigned char FastReader::GetByte() {
    unsigned char value = mData[ mCursor ];

    ++mCursor;

    return value;
}

unsigned short FastReader::GetShort() {
    unsigned short value = *reinterpret_cast<unsigned short*>( mData + mCursor );

    mCursor += sizeof(short);

    return value;
}

int FastReader::GetInteger() {
    int value = *reinterpret_cast<int *>( mData + mCursor );

    mCursor += sizeof(int);

    return value;
}

FastReader::~FastReader() {
    delete [] mData;
}

bool FastReader::ReadFile( const string & fileName ) {
    ifstream file( fileName, ios_base::binary );
    if ( file.bad() ) {
        return false;
    }
	file.seekg( 0, ios_base::end );
    mSize = file.tellg();
    mData = new Byte[mSize];
    file.seekg( 0, ios_base::beg );
    file.read( reinterpret_cast<char*>( mData ), mSize );
	file.close();
    return true;
}

FastReader::FastReader() : mSize( 0 ), mCursor( 0 ), mData( 0 ) {

}
