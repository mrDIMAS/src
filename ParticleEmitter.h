#pragma once

#include "Common.h"
#include "SceneNode.h"

class ParticleEmitter {
public:
    class SParticleVertex {
    public:
        float x, y, z;
        float tx, ty;
        int color;

        SParticleVertex( Vector3 thePosition, float theTextureCoordX, float theTextureCoordY, int theColor );
        SParticleVertex( );
    };

    class SParticleFace {
    public:
        short v1;
        short v2;
        short v3;

        SParticleFace( short theFirstVertex, short theSecondVertex, short theThirdVertex );

        SParticleFace( );
    };

    class SParticle {
    public:
        Vector3 position;
        Vector3 color;
        Vector3 speed;
        float translucency;
        float size;

        SParticle( const Vector3 & thePosition, const Vector3 & theSpeed, const Vector3 & theColor, float theTranslucency, float theSize );
        SParticle( );
    };

    class ZSorter {
    public:
        D3DXMATRIX worldView;

        bool operator() ( SParticle & p1, SParticle & p2 );
    };

    vector< SParticle > particles;

    SParticleVertex * vertices;
    SParticleFace * faces;

    IDirect3DVertexBuffer9 * vertexBuffer;
    IDirect3DIndexBuffer9 * indexBuffer;

	SceneNode * base;

    int aliveParticles;

    float boundaryLayerThickness;
	D3DXMATRIX world;
    ZSorter zSorter;

    bool firstTimeUpdate;

	ParticleSystemProperties props;

	int RGBAToInt( Vector3 color, int alpha );
	Vector3 RandomVector3( Vector3 & min, Vector3 & max );
public:
    ParticleEmitter( SceneNode * theParent, int theParticleCount, ParticleSystemProperties creationProps );
    ~ParticleEmitter();
    void ResurrectParticles();
    float GetThickness( );
    bool IsEnabled();
    bool HasAliveParticles();    
    void ResurrectParticle( SParticle & p );    
    bool IsLightAffects( );
    SceneNode * GetBase();
    void Update( );
    void Bind();
    void Render();
};