#pragma once

class Vertex {
public:
    ruVector3 mPosition;
    ruVector3 mNormal;
    ruVector2 mTexCoord;
    ruVector3 mTangent;

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