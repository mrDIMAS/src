#include "TextFileStream.h"




void TextFileStream::WriteString( string str ) {
    mStream << str << endl;
}

void TextFileStream::WriteFloat( float fl ) {
    mStream << fl << endl;
}

void TextFileStream::WriteInteger( int i ) {
    mStream << i << endl;
}

void TextFileStream::WriteBoolean( bool b ) {
    mStream << b << endl;
}

void TextFileStream::WriteVector3( ruVector3 v ) {
    WriteFloat( v.x );
    WriteFloat( v.y );
    WriteFloat( v.z );
}

void TextFileStream::WriteQuaternion( ruQuaternion q ) {
    WriteFloat( q.x );
    WriteFloat( q.y );
    WriteFloat( q.z );
    WriteFloat( q.w );
}

string TextFileStream::Readstring() {
    string str;
    mStream >> str;
    return str;
}

void TextFileStream::Readstring( string & str ) {
    getline( mStream, str );
}

float TextFileStream::ReadFloat() {
    float fl;
    mStream >> fl;
    return fl;
}

void TextFileStream::ReadFloat( float & f ) {
    mStream >> f;
}

int TextFileStream::ReadInteger() {
    int i;
    mStream >> i;
    return i;
}

void TextFileStream::ReadInteger( int & i ) {
    mStream >> i;
}

bool TextFileStream::ReadBoolean() {
    bool b;
    mStream >> b;
    return b;
}

void TextFileStream::ReadBoolean( bool & b ) {
    mStream >> b;
}

ruVector3 TextFileStream::ReadVector3() {
    ruVector3 v;
    mStream >> v.x;
    mStream >> v.y;
    mStream >> v.z;
    return v;
}

void TextFileStream::ReadVector3( ruVector3 & v ) {
    mStream >> v.x;
    mStream >> v.y;
    mStream >> v.z;
}

ruQuaternion TextFileStream::ReadQuaternion() {
    ruQuaternion q;
    mStream >> q.x;
    mStream >> q.y;
    mStream >> q.z;
    mStream >> q.w;
    return q;
}

void TextFileStream::ReadQuaternion( ruQuaternion & q ) {
    mStream >> q.x;
    mStream >> q.y;
    mStream >> q.z;
    mStream >> q.w;
}

TextFileStream::~TextFileStream() {
    mStream.flush();
    mStream.close();
}

TextFileStream::TextFileStream( const char * fileName, bool save ) {
    int flags = save ? ( fstream::out | fstream::trunc ) : ( fstream::in );
    mStream.open( fileName, flags );
}
