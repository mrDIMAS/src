#pragma once

#include "AABB.h"

class Octree;
class Texture;

class Mesh : public RendererComponent {
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

        Triangle( unsigned short vA, unsigned short vB, unsigned short vC );
    };

	struct RealBone {
		SceneNode * node;
		btTransform transform;
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
    // mesh can be shared between multiply nodes
	vector<SceneNode*> mOwnerList;
    AABB mAABB;
    Octree * mOctree;
    float mOpacity;
	static IDirect3DVertexDeclaration9 * msVertexDeclaration;
public:
	void OnResetDevice();
	void OnLostDevice();
    static unordered_map< IDirect3DTexture9*, vector< Mesh*>> msMeshList;
    static void Register( Mesh * mesh );
	static void EraseOrphanMeshes();
	static void CleanUp();
    explicit Mesh();
    virtual ~Mesh();
	void LinkTo( SceneNode * owner );
	void Unlink( SceneNode * owner );
    vector<SceneNode*> & GetOwners();
    void UpdateVertexBuffer();
    void UpdateIndexBuffer( vector< Triangle > & idc );
    void UpdateBuffers();
    Texture * GetDiffuseTexture();
    Texture * GetNormalTexture();
    void BindBuffers();
    void Render();
	void RenderEx( IDirect3DIndexBuffer9 * ib, int faceCount );
};