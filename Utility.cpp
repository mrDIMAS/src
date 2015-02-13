#include "Utility.h"

void GetD3DMatrixFromBulletTransform ( const btTransform & trn, D3DMATRIX & outMatrix ) {
    btVector3 R = trn.getBasis().getColumn ( 0 );
    btVector3 U = trn.getBasis().getColumn ( 1 );
    btVector3 L = trn.getBasis().getColumn ( 2 );
    btVector3 P = trn.getOrigin();
    outMatrix._11 = R.x();
    outMatrix._12 = R.y();
    outMatrix._13 = R.z();
    outMatrix._14 = 0.0f;
    outMatrix._21 = U.x();
    outMatrix._22 = U.y();
    outMatrix._23 = U.z();
    outMatrix._24 = 0.0f;
    outMatrix._31 = L.x();
    outMatrix._32 = L.y();
    outMatrix._33 = L.z();
    outMatrix._34 = 0.0f;
    outMatrix._41 = P.x();
    outMatrix._42 = P.y();
    outMatrix._43 = P.z();
    outMatrix._44 = 1.0f;
}

std::string Format( string format, ... ) {
    char buffer[ 1024 ];
    va_list	argumentList;

    va_start( argumentList, format );
    vsprintf_s( buffer, format.c_str(), argumentList);
    va_end(argumentList);

    return string( buffer );
}

float frandom( float low, float high ) {
    return low + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(high-low)));
}

int GetNearestPow2( int num ) {
    if( num >= 3072 && num < 4096 ) {
        return 4096;
    }
    if( num >= 2048 && num < 3072 ) {
        return 2048;
    }
    if( num >= 1536 && num < 2048 ) {
        return 2048;
    }
    if( num >= 1024 && num < 1536 ) {
        return 1024;
    }
    if( num >= 768 && num < 1024 ) {
        return 1024;
    }
    if( num >= 512 && num < 768 ) {
        return 512;
    }
    if( num >= 384 && num < 512 ) {
        return 512;
    }
    if( num >= 256 && num < 384 ) {
        return 256;
    }
    if( num >= 192 && num < 256 ) {
        return 256;
    }
    if( num >= 128 && num < 192 ) {
        return 128;
    }
    if( num >= 96 && num < 128 ) {
        return 128;
    }
    if( num >= 64 && num < 96 ) {
        return 64;
    }
    if( num >= 48 && num < 64 ) {
        return 64;
    }
    if( num >= 32 && num < 48 ) {
        return 32;
    }
    if( num >= 24 && num < 32 ) {
        return 32;
    }
    if( num >= 16 && num < 24 ) {
        return 16;
    }

    return 8;
}
