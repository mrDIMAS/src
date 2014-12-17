#pragma once

#include "Common.h"

class Writer
{
private:
  ofstream output;

public:
  Writer( string fn )
  {    
    output.open( fn, ios::binary | ios::out );
  }  

  ~Writer()
  {
    output.close();
  }

  void WriteInteger( int value )
  {
    output.write( (char*)&value, sizeof( value ));
  }

  void WriteShort( unsigned short value )
  {
    output.write( (char*)&value, sizeof( value ));
  }

  void WriteFloat( float value )
  {
    output.write( (char*)&value, sizeof( value ));
  }

  void WriteString( string value )
  {
    value.push_back( 0 ) ;
    output.write( (char*)&value[ 0 ], value.size() );
  }

  void WriteVector( Vector3 v )
  {
    WriteFloat( v.x );
    WriteFloat( v.y );
    WriteFloat( v.z );
  }

  void WriteVector2( Vector2 v )
  {
    WriteFloat( v.x );
    WriteFloat( v.y );
  }

  void WriteQuaternion( Vector4 v )
  {
    WriteFloat( v.x );
    WriteFloat( v.y );
    WriteFloat( v.z );
    WriteFloat( v.w );
  }
};