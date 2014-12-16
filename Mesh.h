#pragma once

#include "Common.h"
#include "AABB.h"

class Octree;

class Mesh {
public:
    class Bone {
    public:
        float weight;
        int id;
    };

    class Weight {
    public:
        Bone bones[ 4 ];
        int boneCount;
    };

    class Triangle {
    public:
        unsigned short a;
        unsigned short b;
        unsigned short c;

        Triangle( unsigned short vA, unsigned short vB, unsigned short vC ) {
            a = vA;
            b = vB;
            c = vC;
        }
    };

public:
    IDirect3DVertexBuffer9 * vertexBuffer;
    IDirect3DIndexBuffer9 * indexBuffer;
    Texture * diffuseTexture;
    Texture * normalMapTexture;
    vector<Vertex> vertices;
    vector<Triangle> triangles;
    vector<Weight> weightTable;
    vector<Vertex> skinningBuffer;
    SceneNode * ownerNode;
    AABB aabb;
    Octree * octree;
    float opacity;
public:
    static unordered_map< IDirect3DTexture9*, vector< Mesh*>> meshes;
    static void Register( Mesh * mesh );
    explicit Mesh( SceneNode * theParent );
    virtual ~Mesh();
    SceneNode * GetParentNode();
    void UpdateVertexBuffer();
    void UpdateIndexBuffer( vector< Triangle > & idc );
    void UpdateBuffers();
    Texture * GetDiffuseTexture();
    Texture * GetNormalTexture();
    void BindBuffers();
    void Render();
};