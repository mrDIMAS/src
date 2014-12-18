#pragma once

#include "Common.h"
#include "Vector.h"

struct Vertex
{
  Vector3 * pos;
  Vector3 nor;
  Vector2 * tex;
  Vector2 * secondTex;
  Vector3 tan;
  Vertex( Vector3 * _pos, Vector2 * _tex, Vector2 * _secondTex, Vector3 normal );
  Vertex( const Vertex & v );
};