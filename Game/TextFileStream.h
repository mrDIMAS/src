#pragma once

#include "Game.h"

class TextFileStream
{
private:
  fstream stream;
public:
  TextFileStream( const char * fileName )
  {
    stream.open( fileName );
  }

  ~TextFileStream()
  {
    stream.close();
  }

  // Read functions
  void ReadQuaternion( Quaternion & q )
  {
    stream >> q.x;
    stream >> q.y;
    stream >> q.z;
    stream >> q.w;
  }

  void ReadVector3( Vector3 & v )
  {
    stream >> v.x;
    stream >> v.y;
    stream >> v.z;
  }

  void ReadBoolean( bool & b )
  {
    stream >> b;
  }

  void ReadInteger( int & i )
  {
    stream >> i;
  }

  void ReadFloat( float & f )
  {
    stream >> f;
  }

  void ReadString( std::string & str )
  {
    stream >> str;
  }


  Quaternion ReadQuaternion()
  {
    Quaternion q;
    stream >> q.x;
    stream >> q.y;
    stream >> q.z;
    stream >> q.w;
    return q;
  }

  Vector3 ReadVector3()
  {
    Vector3 v;
    stream >> v.x;
    stream >> v.y;
    stream >> v.z;
    return v;
  }

  bool ReadBoolean()
  {
    bool b;
    stream >> b;
    return b;
  }

  int ReadInteger()
  {
    int i;
    stream >> i;
    return i;
  }

  float ReadFloat()
  {
    float fl;
    stream >> fl;
    return fl;
  }

  std::string ReadString()
  {
    string str;
    stream >> str;
    return str;
  }

  // Write functions
  void WriteQuaternion( Quaternion q )
  {
    WriteFloat( q.x );
    WriteFloat( q.y );
    WriteFloat( q.z );
    WriteFloat( q.w );
    stream << endl;
  }

  void WriteVector3( Vector3 v )
  {
    WriteFloat( v.x );
    WriteFloat( v.y );
    WriteFloat( v.z );
    stream << endl;
  }

  void WriteBoolean( bool b )
  {
    stream << b << endl;
  }

  void WriteInteger( int i )
  {
    stream << i << endl;
  }

  void WriteFloat( float fl )
  {
    stream << fl << endl;
  }

  void WriteString( string str )
  {
    stream << str << endl;
  }
};