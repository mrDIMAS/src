#include "Precompiled.h"

#include "SaveFile.h"

void SaveFile::WriteString( string str ) {
    mStream << str << endl;
}

void SaveFile::WriteFloat( float fl ) {
    mStream << fl << endl;
}

void SaveFile::WriteInteger( int i ) {
    mStream << i << endl;
}

void SaveFile::WriteBoolean( bool b ) {
    mStream << b << endl;
}

void SaveFile::WriteVector3( ruVector3 v ) {
    WriteFloat( v.x );
    WriteFloat( v.y );
    WriteFloat( v.z );
}

void SaveFile::WriteQuaternion( ruQuaternion q ) {
    WriteFloat( q.x );
    WriteFloat( q.y );
    WriteFloat( q.z );
    WriteFloat( q.w );
}

string SaveFile::ReadString() {
    string str;
	if( mStream.peek() == '\n' ) {
		mStream.ignore( 1 );
	}
    getline( mStream, str );
    return str;
}

void SaveFile::ReadString( string & str ) {
    getline( mStream, str );
}

float SaveFile::ReadFloat() {
    float fl;
    mStream >> fl;
    return fl;
}

void SaveFile::ReadFloat( float & f ) {
    mStream >> f;
}

int SaveFile::ReadInteger() {
    int i = 0;
    mStream >> i;
    return i;
}

void SaveFile::ReadInteger( int & i ) {
    mStream >> i;
}

bool SaveFile::ReadBoolean() {
    bool b;
    mStream >> b;
    return b;
}

void SaveFile::ReadBoolean( bool & b ) {
    mStream >> b;
}

ruVector3 SaveFile::ReadVector3() {
    ruVector3 v;
    mStream >> v.x;
    mStream >> v.y;
    mStream >> v.z;
    return v;
}

void SaveFile::ReadVector3( ruVector3 & v ) {
    mStream >> v.x;
    mStream >> v.y;
    mStream >> v.z;
}

ruQuaternion SaveFile::ReadQuaternion() {
    ruQuaternion q;
    mStream >> q.x;
    mStream >> q.y;
    mStream >> q.z;
    mStream >> q.w;
    return q;
}

void SaveFile::ReadQuaternion( ruQuaternion & q ) {
    mStream >> q.x;
    mStream >> q.y;
    mStream >> q.z;
    mStream >> q.w;
}

SaveFile::~SaveFile() {
    mStream.flush();
    mStream.close();
}

SaveFile::SaveFile( const string & fileName, bool save ) {
    int flags = save ? ( fstream::out | fstream::trunc ) : ( fstream::in );
    mStream.open( fileName, flags );
	if( mStream.bad() ) {
		RaiseError( StringBuilder( "Unable to open " ) << fileName << " save file!" );
	}
}
