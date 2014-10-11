#pragma once

#include "Common.h"

class AABB
{
public:
  Vector3 vertices[ 8 ];
  Vector3 min;
  Vector3 max;
  AABB( );
  AABB( const Vector3 & min, const Vector3 & max );
  bool IsPointInside( const Vector3 & v );
  bool IsTriangleInside( const Vector3 & v1, const Vector3 & v2, const Vector3 & v3 );
  void Visualize();
};