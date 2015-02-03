#pragma once

#include "Common.h"
#include "SceneNode.h"

class ParticleEmitter {
public:
    class ParticleVertex {
    public:
        float x, y, z;
        float tx, ty;
        int color;

        ParticleVertex( ruVector3 thePosition, float theTextureCoordX, float theTextureCoordY, int theColor );
        ParticleVertex( );
    };

    class ParticleFace {
    public:
        short v1;
        short v2;
        short v3;

        explicit ParticleFace( short theFirstVertex, short theSecondVertex, short theThirdVertex );
        ParticleFace( );
    };

    class Particle {
    public:
        ruVector3 position;
        ruVector3 color;
        ruVector3 speed;
        float mOpacity;
        float size;

        explicit Particle( const ruVector3 & thePosition, const ruVector3 & theSpeed, const ruVector3 & theColor, float theTranslucency, float theSize );
        Particle( );
    };

    vector< Particle > particles;

    ParticleVertex * vertices;
    ParticleFace * faces;

    IDirect3DVertexBuffer9 * vertexBuffer;
    IDirect3DIndexBuffer9 * indexBuffer;

    SceneNode * mOwner;

    int mAliveParticleCount;

	D3DXMATRIX mWorldTransform;

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
    void ResurrectParticle( Particle & p );
    bool IsLightAffects( );
    SceneNode * GetOwner();
    void Update( );
    void Bind();
    void Render();
};