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

Vertex::Vertex( Vector3 & theCoords, Vector3 & theNormals, Vector2 & theTexCoords, Vector3 & theTangents, Vector2 & theTexCoords2 ) {
    coords = theCoords;
    normals = theNormals;
    texCoords = theTexCoords;
    tangents = theTangents;
    texCoords2 = theTexCoords2;
}


Vertex2D::Vertex2D( float x, float y, float z, float tx, float ty, int color /*= 0 */ ) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->tx = tx;
    this->ty = ty;
    this->color = color;
}
