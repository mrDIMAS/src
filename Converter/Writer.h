#pragma once

#include "Common.h"
#include "Vector.h"

class Writer
{
private:
  ofstream output;
public:
  Writer( string fn );  
  ~Writer();
  void WriteInteger( int value );
  void WriteShort( unsigned short value );
  void WriteFloat( float value );
  void WriteString( string value );
  void WriteVector( Vector3 v );
  void WriteVector2( Vector2 v );
  void WriteQuaternion( Vector4 v );
};