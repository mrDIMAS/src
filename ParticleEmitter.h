#pragma once

#include "Common.h"
#include "SceneNode.h"

class ParticleEmitter
{
private:
  class SParticleVertex
  {
  public:
    float x, y, z; 
    float tx, ty;
    int color;

    SParticleVertex( float theX, float theY, float theZ, float theTextureCoordX, float theTextureCoordY, int theColor );

    SParticleVertex( Vector3 thePosition, float theTextureCoordX, float theTextureCoordY, int theColor );

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

  Texture * texture;
  SceneNode * base;

  int aliveParticles;
  int type;

  Vector3 boundingBoxMin;
  Vector3 boundingBoxMax;

  float boundingSphereRadius;

  Vector3 speedDeviationMin;
  Vector3 speedDeviationMax;

  bool autoResurrect;
  float boundaryLayerThickness;

  float thickness;

  Vector3 colorBegin;
  Vector3 colorEnd;

  D3DXMATRIX world;

  bool lightAffects;
  bool enabled;

  float pointSize;

  ZSorter zSorter;

  float scaling;

  bool firstTimeUpdate;
public:
  ParticleEmitter( SceneNode * theParent, int theParticleCount, Texture * theTexture, int theType = PS_BOX );
  ~ParticleEmitter();
  void ResurrectParticles();
  float GetThickness( );
  bool IsEnabled();
  bool HasAliveParticles();
  Vector3 RandomVector3( Vector3 & min, Vector3 & max );
  void SetScaling( float scl );
  void ResurrectParticle( SParticle & p );
  int RGBAToInt( Vector3 color, int alpha );
  float GetBox1DTranslucency( float coord, float maxCoord, float minCoord );
  void SetRadius( float theRadius );
  float GetBoxBoundaryLayerTranslucency( const SParticle & particle );
  float GetSphereBoundaryLayerTranslucency( const SParticle & particle );
  float CalculateTranslucency( const SParticle & particle );
  float GetBox1DColorInterpolationCoefficient( float coord, float maxCoord, float minCoord );
  float GetBoxColorInterpolationCoefficient( const SParticle & particle );
  float GetSphereColorInterpolationCoefficient( const SParticle & particle );
  float CalculateColorInterpolationCoefficient( const SParticle & particle );
  bool IsLightAffects( );
  SceneNode * GetBase();
  void Update( );
  void SetLighting( bool state );
  void SetColors( const Vector3 & theBeginColor, const Vector3 & theEndColor );
  void SetParticleSize( float thePointSize );
  D3DXMATRIX GetWorldTransform();
  void SetAutoResurrection( bool state );
  int GetAliveParticles();
  void SetEnabled( bool state );
  void SetSpeedDeviation( const Vector3 & theMin, const Vector3 & theMax );
  void SetThickness( float theThickness );
  void SetBoundingBox( const Vector3 & theMin, const Vector3 & theMax );
  void Bind();
  void Render();  
};