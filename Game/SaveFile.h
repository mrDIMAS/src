#pragma once

#include "Game.h"

class SaveFile {
private:
    fstream mStream;
public:
    SaveFile( const string & fileName, bool save );
    ~SaveFile();

    // Read functions
    void ReadQuaternion( ruQuaternion & q );
    void ReadVector3( ruVector3 & v );
    void ReadBoolean( bool & b );
    void ReadInteger( int & i );
    void ReadFloat( float & f );
    void ReadString( string & str );

    ruQuaternion ReadQuaternion();
    ruVector3 ReadVector3();
    bool ReadBoolean();
    int ReadInteger();
    float ReadFloat();
    string ReadString();

    // Write functions
    void WriteQuaternion( ruQuaternion q );
    void WriteVector3( ruVector3 v );
    void WriteBoolean( bool b );
    void WriteInteger( int i );
    void WriteFloat( float fl );
    void WriteString( string str );
};