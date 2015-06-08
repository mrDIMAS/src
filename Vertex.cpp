#include "Precompiled.h"

#include "Vertex.h"

Vertex::Vertex() {

}

Vertex::Vertex( float x, float y, float z, float tx, float ty ) {
    mPosition.x = x;
    mPosition.y = y;
    mPosition.z = z;

    mTexCoord.x = tx;
    mTexCoord.y = ty;
}

Vertex::Vertex( ruVector3 & theCoords, ruVector3 & theNormals, ruVector2 & theTexCoords, ruVector3 & theTangents ) {
    mPosition = theCoords;
    mNormal = theNormals;
    mTexCoord = theTexCoords;
    mTangent = theTangents;
}


Vertex2D::Vertex2D( float x, float y, float z, float tx, float ty, int color /*= 0 */ ) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->tx = tx;
    this->ty = ty;
    this->color = color;
}
