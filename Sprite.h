#pragma once

#include "SceneNode.h"
#include "Mesh.h"
#include "Vertex.h"

class Sprite : public SceneNode {
private:
    Mesh * quad;
public:
    Sprite() {
        quad = new Mesh( this );

        quad->vertices.push_back( Vertex( -0.5f, -0.5f, 0.0f, 0.0f, 0.0f ) );
        quad->vertices.push_back( Vertex(  0.5f, -0.5f, 0.0f, 1.0f, 0.0f ) );
        quad->vertices.push_back( Vertex(  0.5f,  0.5f, 0.0f, 1.0f, 1.0f ) );
        quad->vertices.push_back( Vertex( -0.5f,  0.5f, 0.0f, 0.0f, 1.0f ) );

        quad->triangles.push_back( 0 );
        quad->triangles.push_back( 1 );
        quad->triangles.push_back( 2 );

        quad->triangles.push_back( 0 );
        quad->triangles.push_back( 2 );
        quad->triangles.push_back( 3 );

        quad->UpdateBuffers();

        Mesh::Register( quad );

        meshes.push_back( quad );
    }

    void SetTexture( ) {
        //quad->
    }
};