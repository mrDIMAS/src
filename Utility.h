#pragma once

#include "Common.h"

void GetD3DMatrixFromBulletTransform ( const btTransform & trn, D3DMATRIX & outMatrix );

float frandom( float low, float high );
int GetNearestPow2( int num );;