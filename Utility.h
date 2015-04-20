#pragma once

#include "Common.h"

void GetD3DMatrixFromBulletTransform ( const btTransform & trn, D3DMATRIX & outMatrix );
