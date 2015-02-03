#pragma once

#include "Common.h"
#include "AABB.h"

class Octree;

class Mesh {
public:
    class Bone {
    public:
        float mWeight;
        int mID;
    };

    class Weight {
    public:
        Bone mBone[ 4 ];
        int mBoneCount;
    };

    class Triangle {
    public:
        unsigned short mA;
        unsigned short mB;
        unsigned short mC;

        Triangle( unsigned short vA, unsigned short vB, unsigned short vC ) {
            mA = vA;
            mB = vB;
            mC = vC;
        }
    };

public:
    IDirect3DVertexBuffer9 * mVertexBuffer;
    IDirect3DIndexBuffer9 * mIndexBuffer;
    Texture * mDiffuseTexture;
    Texture * mNormalTexture;
    vector<Vertex> mVertices;
    vector<Triangle> mTriangles;
    vector<Weight> mWeightTable;
    vector<Vertex> mSkinVertices;
    SceneNode * mOwnerNode;
    AABB mAABB;
    Octree * mOctree;
    float mOpacity;
public:
    static unordered_map< IDirect3DTexture9*, vector< Mesh*>> msMeshList;
    static void Register( Mesh * mesh );
    explicit Mesh( SceneNode * theParent );
    virtual ~Mesh();
    SceneNode * GetOwner();
    void UpdateVertexBuffer();
    void UpdateIndexBuffer( vector< Triangle > & idc );
    void UpdateBuffers();
    Texture * GetDiffuseTexture();
    Texture * GetNormalTexture();
    void BindBuffers();
    void Render();
};