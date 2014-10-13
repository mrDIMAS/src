#include "TextFileStream.h"




void TextFileStream::WriteString( string str )
{
  stream << str << endl;
}

void TextFileStream::WriteFloat( float fl )
{
  stream << fl << endl;
}

void TextFileStream::WriteInteger( int i )
{
  stream << i << endl;
}

void TextFileStream::WriteBoolean( bool b )
{
  stream << b << endl;
}

void TextFileStream::WriteVector3( Vector3 v )
{
  WriteFloat( v.x );
  WriteFloat( v.y );
  WriteFloat( v.z );
}

void TextFileStream::WriteQuaternion( Quaternion q )
{
  WriteFloat( q.x );
  WriteFloat( q.y );
  WriteFloat( q.z );
  WriteFloat( q.w );
}

std::string TextFileStream::ReadString()
{
  string str;
  stream >> str;
  return str;
}

void TextFileStream::ReadString( std::string & str )
{
  stream >> str;
}

float TextFileStream::ReadFloat()
{
  float fl;
  stream >> fl;
  return fl;
}

void TextFileStream::ReadFloat( float & f )
{
  stream >> f;
}

int TextFileStream::ReadInteger()
{
  int i;
  stream >> i;
  return i;
}

void TextFileStream::ReadInteger( int & i )
{
  stream >> i;
}

bool TextFileStream::ReadBoolean()
{
  bool b;
  stream >> b;
  return b;
}

void TextFileStream::ReadBoolean( bool & b )
{
  stream >> b;
}

Vector3 TextFileStream::ReadVector3()
{
  Vector3 v;
  stream >> v.x;
  stream >> v.y;
  stream >> v.z;
  return v;
}

void TextFileStream::ReadVector3( Vector3 & v )
{
  stream >> v.x;
  stream >> v.y;
  stream >> v.z;
}

Quaternion TextFileStream::ReadQuaternion()
{
  Quaternion q;
  stream >> q.x;
  stream >> q.y;
  stream >> q.z;
  stream >> q.w;
  return q;
}

void TextFileStream::ReadQuaternion( Quaternion & q )
{
  stream >> q.x;
  stream >> q.y;
  stream >> q.z;
  stream >> q.w;
}

TextFileStream::~TextFileStream()
{
  stream.close();
}

TextFileStream::TextFileStream( const char * fileName, bool save )
{
  int flags = save ? ( fstream::out | fstream::trunc ) : ( fstream::in );
  stream.open( fileName, flags );
}
