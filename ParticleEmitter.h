#pragma once

#include "Common.h"
#include "SceneNode.h"

class ParticleEmitter
{
public:
    class SParticleVertex
    {
    public:
        float x, y, z;
        float tx, ty;
        int color;

        SParticleVertex( ruVector3 thePosition, float theTextureCoordX, float theTextureCoordY, int theColor );
        SParticleVertex( );
    };

    class SParticleFace
    {
    public:
        short v1;
        short v2;
        short v3;

        SParticleFace( short theFirstVertex, short theSecondVertex, short theThirdVertex );

        SParticleFace( );
    };

    class SParticle
    {
    public:
        ruVector3 position;
        ruVector3 color;
        ruVector3 speed;
        float translucency;
        float size;

        SParticle( const ruVector3 & thePosition, const ruVector3 & theSpeed, const ruVector3 & theColor, float theTranslucency, float theSize );
        SParticle( );
    };

    class ZSorter
    {
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

    ruParticleSystemProperties props;

    int RGBAToInt( ruVector3 color, int alpha );
    ruVector3 RandomVector3( ruVector3 & min, ruVector3 & max );
public:
    ParticleEmitter( SceneNode * theParent, int theParticleCount, ruParticleSystemProperties creationProps );
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