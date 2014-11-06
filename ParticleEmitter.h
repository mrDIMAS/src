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

        SParticleVertex( float theX, float theY, float theZ, float theTextureCoordX, float theTextureCoordY, int theColor );

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
    private:
        Vector3 position;
        Vector3 color;
        Vector3 speed;
        float translucency;
        float size;

    public:
        SParticle( const Vector3 & thePosition, const Vector3 & theSpeed, const Vector3 & theColor, float theTranslucency, float theSize );
        SParticle( );
        void SetPosition( const Vector3 & thePosition );
        void SetPosition( const btVector3 & thePosition );
        Vector3 GetPosition( ) const;
        void SetSpeed( const Vector3 & theSpeed );
        Vector3 GetSpeed( ) const;
        void SetColor( const Vector3 & theColor );
        Vector3 GetColor( ) const;
        void SetTranslucency( float theTranslucency );
        float GetTranslucency();
        void SetSize( float theSize );
        float GetSize();
        void Move( );
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

	float GetBoxColorInterpolationCoefficient( const SParticle & particle );
	float GetSphereColorInterpolationCoefficient( const SParticle & particle );
	float CalculateColorInterpolationCoefficient( const SParticle & particle );
	float GetBoxBoundaryLayerTranslucency( const SParticle & particle );
	float GetSphereBoundaryLayerTranslucency( const SParticle & particle );
	float CalculateTranslucency( const SParticle & particle );
	float GetBox1DColorInterpolationCoefficient( float coord, float maxCoord, float minCoord );
	float GetBox1DTranslucency( float coord, float maxCoord, float minCoord );
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
    D3DXMATRIX GetWorldTransform();
    int GetAliveParticles();
    void Bind();
    void Render();
};