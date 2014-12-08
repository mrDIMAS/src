#pragma once

#include "Game.h"

class TextFileStream {
private:
    fstream stream;
public:
    TextFileStream( const char * fileName, bool save );
    ~TextFileStream();

    // Read functions
    void ReadQuaternion( ruQuaternion & q );
    void ReadVector3( ruVector3 & v );
    void ReadBoolean( bool & b );
    void ReadInteger( int & i );
    void ReadFloat( float & f );
    void Readstring( string & str );

    ruQuaternion ReadQuaternion();
    ruVector3 ReadVector3();
    bool ReadBoolean();
    int ReadInteger();
    float ReadFloat();
    string Readstring();

    // Write functions
    void WriteQuaternion( ruQuaternion q );
    void WriteVector3( ruVector3 v );
    void WriteBoolean( bool b );
    void WriteInteger( int i );
    void WriteFloat( float fl );
    void Writestring( string str );
};