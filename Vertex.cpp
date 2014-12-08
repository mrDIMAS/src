#include "Vertex.h"

Vertex::Vertex() {

}

Vertex::Vertex( float x, float y, float z, float tx, float ty ) {
    coords.x = x;
    coords.y = y;
    coords.z = z;

    texCoords.x = tx;
    texCoords.y = ty;
}

Vertex::Vertex( ruVector3 & theCoords, ruVector3 & theNormals, ruVector2 & theTexCoords, ruVector3 & theTangents ) {
    coords = theCoords;
    normals = theNormals;
    texCoords = theTexCoords;
    tangents = theTangents;
}


Vertex2D::Vertex2D( float x, float y, float z, float tx, float ty, int color /*= 0 */ ) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->tx = tx;
    this->ty = ty;
    this->color = color;
}
