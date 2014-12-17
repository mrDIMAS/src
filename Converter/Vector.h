#pragma once

#include "Common.h"

struct Vector4
{
  float x,y,z,w;
};

struct Vector3
{
  float x,y,z;

  Vector3( float _x, float _y, float _z )
  {
    x = _x;
    y = _y;
    z = _z;
  }

  Vector3(  )
  {
  }

  Vector3 cross( const Vector3 & v )
  {
    return Vector3( y * v.z - z * v.x, z * v.x - x * v.z, x * v.y - y * v.x );
  }

  float dot( const Vector3 & v )
  {
    return x * v.x + y * v.y + z * v.z;
  }

  Vector3 normalized( )
  {
    float len = sqrt( x*x + y*y + z*z );
    if( len > 0.000001 )
      return Vector3( x / len, y / len, z / len );
    else
      return Vector3( x, y, z );
  }
};


static inline Vector3 operator - ( const Vector3 & v1, const Vector3 & v2 )
{
  return Vector3( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z );
};

static inline Vector3 operator + ( const Vector3 & v1, const Vector3 & v2 )
{
  return Vector3( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z );
};

static inline const Vector3 operator * ( const Vector3 & v1, const float f )
{
  return Vector3( v1.x * f, v1.y * f, v1.z * f );
};

struct Vector2
{
  float x,y;

  Vector2( float _x, float _y )
  {
    x = _x;
    y = _y;
  }

  Vector2(  )
  {
  }
};