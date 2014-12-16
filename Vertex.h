#pragma once

#include "Common.h"

class Vertex {
public:
    ruVector3 coords;
    ruVector3 normals;
    ruVector2 texCoords;
    ruVector3 tangents;

    Vertex( ruVector3 & theCoords, ruVector3 & theNormals, ruVector2 & theTexCoords, ruVector3 & theTangents );
    Vertex( float x, float y, float z, float tx, float ty );
    Vertex( );
};

class Vertex2D {
public:
    float x, y, z;
    float tx, ty;
    int color;
    Vertex2D( float x, float y, float z, float tx, float ty, int color = 0 );
    Vertex2D( ) {
        x = y = z = tx = ty = 0;
        color = 0xFFFFFFFF;
    }
};