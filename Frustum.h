#pragma once

#include "Common.h"
#include "AABB.h"

class Frustum
{
private:
    D3DXPLANE planes[ 6 ];
public:
    explicit Frustum();
    void Build( D3DXMATRIX viewProjection );
    bool IsPointInside( const ruVector3 & point );
    bool IsAABBInside( const AABB & aabb, const ruVector3 & aabbOffset );
};