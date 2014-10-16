#pragma once

#include "Game.h"

class TextFileStream
{
private:
  fstream stream;
public:
  TextFileStream( const char * fileName, bool save );
  ~TextFileStream();

  // Read functions
  void ReadQuaternion( Quaternion & q );
  void ReadVector3( Vector3 & v );
  void ReadBoolean( bool & b );
  void ReadInteger( int & i );
  void ReadFloat( float & f );
  void Readstring( string & str );

  Quaternion ReadQuaternion();
  Vector3 ReadVector3();
  bool ReadBoolean();
  int ReadInteger();
  float ReadFloat();
  string Readstring();

  // Write functions
  void WriteQuaternion( Quaternion q );
  void WriteVector3( Vector3 v );
  void WriteBoolean( bool b );
  void WriteInteger( int i );
  void WriteFloat( float fl );
  void Writestring( string str );
};