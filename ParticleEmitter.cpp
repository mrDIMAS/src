#include "ParticleEmitter.h"
#include "Camera.h"
#include "Texture.h"
#include "Utility.h"

vector<ParticleEmitter*> g_particleEmitters;

NodeHandle CreateParticleSystem( int particleNum, TextureHandle texture, int type )
{
  SceneNode * node = new SceneNode;
  node->particleEmitter = new ParticleEmitter( node, particleNum, reinterpret_cast<Texture*>( texture.pointer ), type );
  return SceneNode::HandleFromPointer( node );
}

void SetParticleSystemSpeedDeviation( NodeHandle ps, Vector3 min, Vector3 max )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetSpeedDeviation( min, max );
}

void SetParticleSystemColors( NodeHandle ps, Vector3 begin, Vector3 end )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetColors( begin, end );
}

void SetParticleSystemPointSize( NodeHandle ps, float pointSize )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetParticleSize( pointSize );
}

void SetParticleSystemBox( NodeHandle ps, Vector3 min, Vector3 max )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetBoundingBox( min, max );
}

void SetParticleSystemAutoResurrect( NodeHandle ps, int ar )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetAutoResurrection( ar );
}

int GetParticleSystemAliveParticles( NodeHandle ps )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    return n->particleEmitter->GetAliveParticles();

  return 0;
}

void RestartParticleSystem( NodeHandle ps )
{

}

void SetParticleSystemRadius( NodeHandle ps, float radius )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetRadius( radius );
}

void EnableParticleSystem( NodeHandle ps )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetEnabled( 1 );
}

void DisableParticleSystem( NodeHandle ps )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetEnabled( 0 );
}

void SetParticleSystemParticleScaling( NodeHandle ps, float scl )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetScaling( scl );
}

void EnableParticleSystemLighting( NodeHandle ps )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetLighting( 1 );
}

void DisableParticleSystemLighting( NodeHandle ps )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetLighting( 0 );
}

void SetParticleSystemThickness( NodeHandle ps, float thickness )
{
  SceneNode * n = SceneNode::CastHandle( ps );

  if( n->particleEmitter )
    n->particleEmitter->SetThickness( 1 );
}

ParticleEmitter::~ParticleEmitter()
{
  vertexBuffer->Release();
  indexBuffer->Release();

  delete vertices;
  delete faces;

  g_particleEmitters.erase( find( g_particleEmitters.begin(), g_particleEmitters.end(), this ));
}

void ParticleEmitter::Render()
{
  g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, aliveParticles * 4, 0, aliveParticles * 2 );
}

void ParticleEmitter::Bind()
{
  texture->Bind( 0 );
  g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( SParticleVertex ));
  g_device->SetIndices( indexBuffer );
}

void ParticleEmitter::SetBoundingBox( const Vector3 & theMin, const Vector3 & theMax )
{
  boundingBoxMax = theMax;
  boundingBoxMin = theMin;

  ResurrectParticles();
}

void ParticleEmitter::SetThickness( float theThickness )
{
  thickness = theThickness;
}

void ParticleEmitter::SetSpeedDeviation( const Vector3 & theMin, const Vector3 & theMax )
{
  speedDeviationMax = theMax;
  speedDeviationMin = theMin;

  ResurrectParticles();
}

void ParticleEmitter::SetEnabled( bool state )
{
  enabled = state;
}

int ParticleEmitter::GetAliveParticles()
{
  return aliveParticles;
}

void ParticleEmitter::SetAutoResurrection( bool state )
{
  autoResurrect = state;
}

D3DXMATRIX ParticleEmitter::GetWorldTransform()
{
  return world;
}

void ParticleEmitter::SetParticleSize( float thePointSize )
{
  pointSize = thePointSize;
}

void ParticleEmitter::SetColors( const Vector3 & theBeginColor, const Vector3 & theEndColor )
{
  colorBegin = theBeginColor;
  colorEnd = theEndColor;

  ResurrectParticles();
}

void ParticleEmitter::SetLighting( bool state )
{
  lightAffects = state;
}

void ParticleEmitter::Update()
{
  if( !firstTimeUpdate )
  {
    ResurrectParticles();
    firstTimeUpdate = true;
  }

  aliveParticles = 0;

  base->globalTransform.getBasis().setEulerYPR( 0, 0, 0 );
  GetD3DMatrixFromBulletTransform( base->globalTransform, world );
  g_device->SetTransform( D3DTS_WORLD, &world );

  D3DXMATRIX mat = g_camera->view;

  Vector3 rightVect = Vector3( mat._11, mat._21, mat._31 ).Normalize();  
  Vector3 upVect = Vector3( mat._12, mat._22, mat._32 ).Normalize();  

  Vector3 leftTop = upVect - rightVect;
  Vector3 rightTop = upVect + rightVect;
  Vector3 rightBottom = rightVect - upVect;
  Vector3 leftBottom = -( rightVect + upVect );

  int vertexNum = 0, faceNum = 0;

  D3DXMatrixMultiply( &zSorter.worldView, &world, &g_camera->view );
  sort( particles.begin(), particles.end(), zSorter );    

  for( auto pIter = particles.begin(); pIter != particles.end(); ++pIter )
  {
    SParticle & p = *pIter;

    float translucency = CalculateTranslucency( p );
    p.Move();
    p.SetTranslucency( translucency );
    p.SetColor( colorBegin.Lerp( colorEnd , CalculateColorInterpolationCoefficient( p ) ) );
    p.SetSize( p.GetSize() + scaling );

    if( translucency <= 10.0f  )
    {
      if( autoResurrect )
        ResurrectParticle( p );
    } 
    else
    {
      vertices[ vertexNum + 0 ] = SParticleVertex( p.GetPosition() + leftTop * p.GetSize(), 0.0, 0.0, RGBAToInt( p.GetColor(), p.GetTranslucency()));
      vertices[ vertexNum + 1 ] = SParticleVertex( p.GetPosition() + rightTop * p.GetSize(), 1.0, 0.0, RGBAToInt( p.GetColor(), p.GetTranslucency()));
      vertices[ vertexNum + 2 ] = SParticleVertex( p.GetPosition() + rightBottom * p.GetSize(), 1.0, 1.0, RGBAToInt( p.GetColor(), p.GetTranslucency()));
      vertices[ vertexNum + 3 ] = SParticleVertex( p.GetPosition() + leftBottom * p.GetSize(), 0.0, 1.0, RGBAToInt( p.GetColor(), p.GetTranslucency()));

      // indices
      faces[ faceNum + 0 ] = SParticleFace( vertexNum + 0, vertexNum + 1, vertexNum + 3 );
      faces[ faceNum + 1 ] = SParticleFace( vertexNum + 1, vertexNum + 2, vertexNum + 3 );

      ++aliveParticles;

      vertexNum += 4;

      faceNum += 2;
    }
  }

  void * data = 0;
  vertexBuffer->Lock( 0, 0, &data, 0 );
  memcpy( data, vertices, aliveParticles * 4 * sizeof( SParticleVertex ));
  vertexBuffer->Unlock();

  indexBuffer->Lock( 0, 0, &data, 0 );
  memcpy( data, faces, aliveParticles * 2 * sizeof( SParticleFace ));
  indexBuffer->Unlock();
}

SceneNode * ParticleEmitter::GetBase()
{
  return base;
}

bool ParticleEmitter::IsLightAffects()
{
  return lightAffects;
}

float ParticleEmitter::CalculateColorInterpolationCoefficient( const SParticle & particle )
{
  if( type == PS_BOX )
    return GetBoxColorInterpolationCoefficient( particle );
  if( type == PS_STREAM )
    return GetSphereColorInterpolationCoefficient( particle );

  return 0.0f;
}

float ParticleEmitter::GetSphereColorInterpolationCoefficient( const SParticle & particle )
{
  float distance2 = particle.GetPosition().Length2();
  float radius2 = boundingSphereRadius * boundingSphereRadius;

  float coefficient = abs( distance2 / radius2 );
  if( coefficient > 1.0f )
    coefficient = 1.0f;
  return coefficient;
}

float ParticleEmitter::GetBoxColorInterpolationCoefficient( const SParticle & particle )
{
  float xColorInterpolationCoefficient = GetBox1DColorInterpolationCoefficient( particle.GetPosition().x, boundingBoxMax.x, boundingBoxMin.x );
  float yColorInterpolationCoefficient = GetBox1DColorInterpolationCoefficient( particle.GetPosition().y, boundingBoxMax.y, boundingBoxMin.y );
  float zColorInterpolationCoefficient = GetBox1DColorInterpolationCoefficient( particle.GetPosition().z, boundingBoxMax.z, boundingBoxMin.z );

  return ( xColorInterpolationCoefficient + yColorInterpolationCoefficient + zColorInterpolationCoefficient ) / 3.0f ;
}

float ParticleEmitter::GetBox1DColorInterpolationCoefficient( float coord, float maxCoord, float minCoord )
{
  float coefficient = 0;

  if( coord > 0 )
    coefficient = abs( coord / maxCoord );

  if( coord < 0 )
    coefficient = abs( coord / minCoord );

  if( coefficient > 1.0f )
    coefficient = 1.0f;

  return coefficient;
}

float ParticleEmitter::CalculateTranslucency( const SParticle & particle )
{
  if( type == PS_BOX )
    return GetBoxBoundaryLayerTranslucency( particle );
  if( type == PS_STREAM )
    return GetSphereBoundaryLayerTranslucency( particle );
  return 255.0f;
}

float ParticleEmitter::GetSphereBoundaryLayerTranslucency( const SParticle & particle )
{
  float distance2 = particle.GetPosition().Length2();
  float radius2 = boundingSphereRadius * boundingSphereRadius;

  if( distance2 > radius2 )
    return 255.0f * radius2 / ( distance2 );

  return 255.0f;
}

float ParticleEmitter::GetBoxBoundaryLayerTranslucency( const SParticle & particle )
{
  float xTranslucency = GetBox1DTranslucency( particle.GetPosition().x, boundingBoxMax.x, boundingBoxMin.x );
  float yTranslucency = GetBox1DTranslucency( particle.GetPosition().y, boundingBoxMax.y, boundingBoxMin.y );
  float zTranslucency = GetBox1DTranslucency( particle.GetPosition().z, boundingBoxMax.z, boundingBoxMin.z );

  return ( xTranslucency + yTranslucency + zTranslucency ) / 3.0f;
}

void ParticleEmitter::SetRadius( float theRadius )
{
  boundingSphereRadius = theRadius;
}

float ParticleEmitter::GetBox1DTranslucency( float coord, float maxCoord, float minCoord )
{
  float translucency = 1.0f;

  if( coord > maxCoord )
    translucency = abs( maxCoord / coord );

  if( coord < minCoord )
    translucency = abs( coord / minCoord );

  return 255.0f * translucency;
}

int ParticleEmitter::RGBAToInt( Vector3 color, int alpha )
{
  return D3DCOLOR_ARGB( alpha, (int)color.x, (int)color.y, (int)color.z );
}

void ParticleEmitter::ResurrectParticle( SParticle & p )
{
  if( type == PS_BOX )
  {
    p.SetPosition( RandomVector3( boundingBoxMin, Vector3( boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z )  ));
    p.SetSpeed( RandomVector3( speedDeviationMin, speedDeviationMax ));
  }

  if( type == PS_STREAM )
  {
    p.SetPosition( Vector3( 0, 0, 0 ) );
    p.SetSpeed( RandomVector3( speedDeviationMin, speedDeviationMax ));
  }

  p.SetSize( pointSize );
  p.SetTranslucency( 255 );
}

void ParticleEmitter::SetScaling( float scl )
{
  scaling = scl;
}

Vector3 ParticleEmitter::RandomVector3( Vector3 & min, Vector3 & max )
{
  return Vector3( frandom( min.x, max.x ), frandom( min.y, max.y ), frandom( min.z, max.z ) );
}

bool ParticleEmitter::HasAliveParticles()
{
  return aliveParticles > 0;
}

bool ParticleEmitter::IsEnabled()
{
  return enabled;
}

float ParticleEmitter::GetThickness()
{
  return thickness;
}

void ParticleEmitter::ResurrectParticles()
{
  for( size_t i = 0; i < particles.size(); i++ )
    ResurrectParticle( particles.at( i ));
}

ParticleEmitter::ParticleEmitter( SceneNode * theParent, int theParticleCount, Texture * theTexture, int theType /*= PS_BOX */ )
{
  texture = theTexture;
  base = theParent;
  aliveParticles = theParticleCount;
  type = theType;

  scaling = 0.0f;

  g_device->CreateVertexBuffer( theParticleCount * 4 * sizeof( SParticleVertex ), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_MANAGED, &vertexBuffer, 0 );
  g_device->CreateIndexBuffer( theParticleCount * 2 * sizeof( SParticleFace ), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer, 0 );

  faces = new SParticleFace[ theParticleCount * 2 ];
  vertices = new SParticleVertex[ theParticleCount * 4 ];   

  for( int i = 0; i < theParticleCount; i++ )
    particles.push_back( SParticle( ) );

  boundingBoxMin = Vector3( -1.0f, -1.0f, -1.0f );
  boundingBoxMax = Vector3( 1.0f, 1.0f, 1.0f );

  boundingSphereRadius = 50.0f;

  speedDeviationMin = Vector3( -1.0, -1.0, -1.0 );
  speedDeviationMax = Vector3( 1.0, 1.0, 1.0 );

  colorBegin = Vector3( 0.0f, 0.0f, 0.0f );
  colorEnd = Vector3( 255.0f, 255.0f, 255.0f );

  autoResurrect = true;

  boundaryLayerThickness = 5.0f;

  lightAffects = false;
  enabled = true;

  thickness = 1.0f;

  pointSize = 1.0f;

  firstTimeUpdate = false;

  g_particleEmitters.push_back( this );

  ResurrectParticles();
}

bool ParticleEmitter::ZSorter::operator()( SParticle & p1, SParticle & p2 )
{
  D3DXVECTOR3 viewSpacePos1;
  D3DXVec3TransformCoord( &viewSpacePos1, &D3DXVECTOR3( p1.GetPosition().x, p1.GetPosition().y, p1.GetPosition().z ), &worldView );

  D3DXVECTOR3 viewSpacePos2;
  D3DXVec3TransformCoord( &viewSpacePos2, &D3DXVECTOR3( p2.GetPosition().x, p2.GetPosition().y, p2.GetPosition().z ), &worldView );

  return viewSpacePos1.z < viewSpacePos2.z;
}

void ParticleEmitter::SParticle::Move()
{
  position = position + speed;
}

float ParticleEmitter::SParticle::GetSize()
{
  return size;
}

void ParticleEmitter::SParticle::SetSize( float theSize )
{
  size = theSize;
}

float ParticleEmitter::SParticle::GetTranslucency()
{
  return translucency;
}

void ParticleEmitter::SParticle::SetTranslucency( float theTranslucency )
{
  translucency = theTranslucency;
}

Vector3 ParticleEmitter::SParticle::GetColor() const
{
  return color;
}

void ParticleEmitter::SParticle::SetColor( const Vector3 & theColor )
{
  color = theColor;
}

Vector3 ParticleEmitter::SParticle::GetSpeed() const
{
  return speed;
}

void ParticleEmitter::SParticle::SetSpeed( const Vector3 & theSpeed )
{
  speed = theSpeed;
}

Vector3 ParticleEmitter::SParticle::GetPosition() const
{
  return position;
}

void ParticleEmitter::SParticle::SetPosition( const btVector3 & thePosition )
{
  position.x = thePosition.x();
  position.y = thePosition.y();
  position.z = thePosition.z();
}

void ParticleEmitter::SParticle::SetPosition( const Vector3 & thePosition )
{
  position = thePosition;
}

ParticleEmitter::SParticle::SParticle()
{
  SetPosition( Vector3( 0, 0, 0 ) );
  SetSpeed( Vector3( 0, 0, 0 ) );
  SetColor( Vector3( 255, 255, 255 ));
  SetTranslucency( 255 );
  SetSize( 1.0f );
}

ParticleEmitter::SParticle::SParticle( const Vector3 & thePosition, const Vector3 & theSpeed, const Vector3 & theColor, float theTranslucency, float theSize )
{
  SetPosition( thePosition );
  SetSpeed( theSpeed );
  SetColor( theColor );
  SetTranslucency( theTranslucency );
  SetSize( theSize );
}

ParticleEmitter::SParticleFace::SParticleFace()
{
  v1 = 0;
  v2 = 0;
  v3 = 0;
}

ParticleEmitter::SParticleFace::SParticleFace( short theFirstVertex, short theSecondVertex, short theThirdVertex )
{
  v1 = theFirstVertex;
  v2 = theSecondVertex;
  v3 = theThirdVertex;
}

ParticleEmitter::SParticleVertex::SParticleVertex()
{
  x = 0.0f;
  y = 0.0f;
  z = 0.0f;

  tx = 0.0f;
  ty = 0.0f;

  color = 0xFFFFFFFF;
}

ParticleEmitter::SParticleVertex::SParticleVertex( Vector3 thePosition, float theTextureCoordX, float theTextureCoordY, int theColor )
{
  x = thePosition.x;
  y = thePosition.y;
  z = thePosition.z;

  tx = theTextureCoordX;
  ty = theTextureCoordY;

  color = theColor;
}

ParticleEmitter::SParticleVertex::SParticleVertex( float theX, float theY, float theZ, float theTextureCoordX, float theTextureCoordY, int theColor )
{
  x = theX;
  y = theY;
  z = theZ;

  tx = theTextureCoordX;
  ty = theTextureCoordY;

  color = theColor;
}
