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
    return cursor > size;
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

    for ( ; data[ cursor ] != '\0'; ++cursor ) {
        out += data[ cursor ];
    }

    if ( data[ cursor ] == '\0' ) {
        ++cursor;
    }

    return out;
}

float FastReader::GetFloat() {
    float value = *reinterpret_cast<float*>( data + cursor );

    cursor += sizeof(float);

    return value;
}

unsigned char FastReader::GetByte() {
    unsigned char value = data[ cursor ];

    ++cursor;

    return value;
}

unsigned short FastReader::GetShort() {
    unsigned short value = *reinterpret_cast<unsigned short*>( data + cursor );

    cursor += sizeof(short);

    return value;
}

int FastReader::GetInteger() {
    int value = *reinterpret_cast<int *>( data + cursor );

    cursor += sizeof(int);

    return value;
}

FastReader::~FastReader() {
    delete [] data;
}

bool FastReader::ReadFile( const string & fileName ) {
    FILE * f;
    f = fopen( fileName.c_str(), "rb" );

    if ( !f ) {
        return false;
    }

    fseek ( f, 0, SEEK_END );

    size = ftell ( f );

    data = new Byte[size];
    fseek ( f, 0, SEEK_SET );

    fread ( data, 1, size, f );

    fclose ( f );

    return true;
}

FastReader::FastReader() {
    cursor = 0;
    data = 0;
}
