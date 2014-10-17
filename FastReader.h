#ifndef _SDXE_FAST_READER_
#define _SDXE_FAST_READER_

#include "common.h"

class FastReader {
private:
    long size;
    long cursor;
    typedef unsigned char Byte;
    Byte * data;

public:
    FastReader( );
    bool ReadFile ( const char * fileName );
    ~FastReader();
    int GetInteger( );
    unsigned short GetShort( );
    unsigned char GetByte( );
    float GetFloat( );
    string GetString( );
    btVector3 GetVector( );
    Vector3 GetBareVector( );
    Vector2 GetBareVector2( );
    btQuaternion GetQuaternion( );
    bool IsEnded( );
};

#endif
