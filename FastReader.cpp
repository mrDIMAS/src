#include "FastReader.h"




bool FastReader::IsEnded()
{
    return cursor > size;
}

btQuaternion FastReader::GetQuaternion()
{
    float x = GetFloat();
    float y = GetFloat();
    float z = GetFloat();
    float w = GetFloat();

    return btQuaternion( x, y, z, w );
}

ruVector2 FastReader::GetBareVector2()
{
    float x = GetFloat();
    float y = GetFloat();

    return ruVector2( x, y );
}

ruVector3 FastReader::GetBareVector()
{
    float x = GetFloat();
    float y = GetFloat();
    float z = GetFloat();

    return ruVector3( x, y, z );
}

btVector3 FastReader::GetVector()
{
    float x = GetFloat();
    float y = GetFloat();
    float z = GetFloat();

    return btVector3( x, y, z );
}

std::string FastReader::GetString()
{
    string out;

    for ( ; data[ cursor ] != '\0'; ++cursor )
        out += data[ cursor ];

    if ( data[ cursor ] == '\0' )
        ++cursor;

    return out;
}

float FastReader::GetFloat()
{
    float value = * ( float * ) ( data + cursor );

    cursor += 4;

    return value;
}

unsigned char FastReader::GetByte()
{
    unsigned char value = data[ cursor ];

    ++cursor;

    return value;
}

unsigned short FastReader::GetShort()
{
    unsigned short value = * ( unsigned short * ) ( data + cursor );

    cursor += 2;

    return value;
}

int FastReader::GetInteger()
{
    int value = * ( int * ) ( data + cursor );

    cursor += 4;

    return value;
}

FastReader::~FastReader()
{
    delete data;
    data = 0;
}

bool FastReader::ReadFile( const char * fileName )
{
    FILE * f;
    f = fopen( fileName, "rb" );

    if ( !f )
        return false;

    fseek ( f, 0, SEEK_END );

    size = ftell ( f );

    data = new Byte[ size ];

    fseek ( f, 0, SEEK_SET );

    fread ( data, 1, size, f );

    fclose ( f );

    return true;
}

FastReader::FastReader()
{
    cursor = 0;
    data = 0;
}
