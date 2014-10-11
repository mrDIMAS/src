#pragma once

#include "Common.h"

void GetD3DMatrixFromBulletTransform ( const btTransform & trn, D3DMATRIX & outMatrix );
string Format( const char * format, ... );
float frandom( float low, float high );
int GetNearestPow2( int num );;