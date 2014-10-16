#pragma once

#include "Common.h"

class Vertex
{
public:
  Vector3 coords;
  Vector3 normals;
  Vector2 texCoords;
  Vector2 texCoords2;
  Vector3 tangents;

  Vertex( Vector3 & theCoords, Vector3 & theNormals, Vector2 & theTexCoords, Vector3 & theTangents, Vector2 & theTexCoords2 );
  Vertex( float x, float y, float z, float tx, float ty );
  Vertex( );
};

class Vertex2D
{
public:
  float x, y, z;
  float tx, ty;
  int color;
  Vertex2D( float x, float y, float z, float tx, float ty, int color = 0 );
  Vertex2D( )
  {
    x = y = z = tx = ty = 0;
    color = 0xFFFFFFFF;
  }
};