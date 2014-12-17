#pragma once

#include "Common.h"

class Reader
{
public:
  Reader( )
  {
    cursor = 0;
    data = 0;
  }

  bool ReadFile ( string fn )
  {
    FILE * f;
    fopen_s ( &f, fn.c_str(), "rb" );

    if ( !f )
    {
      return false;
    }

    fseek ( f, 0, SEEK_END );

    size = ftell ( f );

    data = new Byte[ size ];

    fseek ( f, 0, SEEK_SET );

    fread ( data, 1, size, f );

    fclose ( f );

    return true;
  }

  ~Reader()
  {
    delete data;

    data = 0;
  }

  int GetInteger( )
  {
    int value = * ( int * ) ( data + cursor );

    cursor += 4;

    return value;
  }

  unsigned short GetShort( )
  {
    unsigned short value = * ( unsigned short * ) ( data + cursor );

    cursor += 2;

    return value;
  }

  unsigned char GetByte( )
  {
    unsigned char value = data[ cursor ];

    ++cursor;

    return value;
  }

  float GetFloat( )
  {
    float value = * ( float * ) ( data + cursor );

    cursor += 4;

    return value;
  }

  string GetString( )
  {
    string out;

    for ( ; data[ cursor ] != '\0'; ++cursor )
    {
      out += data[ cursor ];
    }

    if ( data[ cursor ] == '\0' )
      ++cursor;

    return out;
  }

  Vector3 GetVector( )
  {
    Vector3 v;

    v.x = GetFloat();
    v.y = GetFloat();
    v.z = GetFloat();

    return v;
  }

  Vector2 GetBareVector2( )
  {
    Vector2 v;

    v.x = GetFloat();
    v.y = GetFloat();

    return v;
  }

  Vector4 GetQuaternion( )
  {
    Vector4 v;

    v.x = GetFloat();
    v.y = GetFloat();
    v.z = GetFloat();
    v.w = GetFloat();

    return v;
  }

  bool IsEnded( )
  {
    return cursor > size;
  }
private:
  long size;
  long cursor;
  typedef unsigned char Byte;
  Byte * data;
};