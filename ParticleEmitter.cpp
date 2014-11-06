#include "ParticleEmitter.h"
#include "Camera.h"
#include "Texture.h"
#include "Utility.h"

vector<ParticleEmitter*> g_particleEmitters;

ParticleEmitter::~ParticleEmitter() {
    vertexBuffer->Release();
    indexBuffer->Release();

    delete vertices;
    delete faces;

    g_particleEmitters.erase( find( g_particleEmitters.begin(), g_particleEmitters.end(), this ));
}

void ParticleEmitter::Render() {
    g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, aliveParticles * 4, 0, aliveParticles * 2 );
}

void ParticleEmitter::Bind() {
	Texture * texPtr = (Texture *)props.texture.pointer;
	if( texPtr ) {
		texPtr->Bind( 0 );
	}
    g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( SParticleVertex ));
    g_device->SetIndices( indexBuffer );
}

int ParticleEmitter::GetAliveParticles() {
    return aliveParticles;
}
D3DXMATRIX ParticleEmitter::GetWorldTransform() {
    return world;
}

void ParticleEmitter::Update() {
    if( !firstTimeUpdate ) {
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

    for( auto pIter = particles.begin(); pIter != particles.end(); ++pIter ) {
        SParticle & p = *pIter;

        float translucency = CalculateTranslucency( p );
        p.Move();
        p.SetTranslucency( translucency );
        p.SetColor( props.colorBegin.Lerp( props.colorEnd , CalculateColorInterpolationCoefficient( p ) ) );
        p.SetSize( p.GetSize() + props.scaleFactor );

        if( translucency <= 10.0f  ) {
            if( props.autoResurrectDeadParticles ) {
                ResurrectParticle( p );
            }
        } else {
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

SceneNode * ParticleEmitter::GetBase() {
    return base;
}

bool ParticleEmitter::IsLightAffects() {
    return props.useLighting;
}

float ParticleEmitter::CalculateColorInterpolationCoefficient( const SParticle & particle ) {
    if( props.type == PS_BOX ) {
        return GetBoxColorInterpolationCoefficient( particle );
    }
    if( props.type == PS_STREAM ) {
        return GetSphereColorInterpolationCoefficient( particle );
    }

    return 0.0f;
}

float ParticleEmitter::GetSphereColorInterpolationCoefficient( const SParticle & particle ) {
    float distance2 = particle.GetPosition().Length2();
    float radius2 = props.boundingRadius * props.boundingRadius;

    float coefficient = abs( distance2 / radius2 );
    if( coefficient > 1.0f ) {
        coefficient = 1.0f;
    }
    return coefficient;
}

float ParticleEmitter::GetBoxColorInterpolationCoefficient( const SParticle & particle ) {
    float xColorInterpolationCoefficient = GetBox1DColorInterpolationCoefficient( particle.GetPosition().x, props.boundingBoxMax.x, props.boundingBoxMin.x );
    float yColorInterpolationCoefficient = GetBox1DColorInterpolationCoefficient( particle.GetPosition().y, props.boundingBoxMax.y, props.boundingBoxMin.y );
    float zColorInterpolationCoefficient = GetBox1DColorInterpolationCoefficient( particle.GetPosition().z, props.boundingBoxMax.z, props.boundingBoxMin.z );

    return ( xColorInterpolationCoefficient + yColorInterpolationCoefficient + zColorInterpolationCoefficient ) / 3.0f ;
}

float ParticleEmitter::GetBox1DColorInterpolationCoefficient( float coord, float maxCoord, float minCoord ) {
    float coefficient = 0;

    if( coord > 0 ) {
        coefficient = abs( coord / maxCoord );
    }

    if( coord < 0 ) {
        coefficient = abs( coord / minCoord );
    }

    if( coefficient > 1.0f ) {
        coefficient = 1.0f;
    }

    return coefficient;
}

float ParticleEmitter::CalculateTranslucency( const SParticle & particle ) {
    if( props.type == PS_BOX ) {
        return GetBoxBoundaryLayerTranslucency( particle );
    }
    if( props.type == PS_STREAM ) {
        return GetSphereBoundaryLayerTranslucency( particle );
    }
    return 255.0f;
}

float ParticleEmitter::GetSphereBoundaryLayerTranslucency( const SParticle & particle ) {
    float distance2 = particle.GetPosition().Length2();
    float radius2 = props.boundingRadius * props.boundingRadius;

    if( distance2 > radius2 ) {
        return 255.0f * radius2 / ( distance2 );
    }

    return 255.0f;
}

float ParticleEmitter::GetBoxBoundaryLayerTranslucency( const SParticle & particle ) {
    float xTranslucency = GetBox1DTranslucency( particle.GetPosition().x, props.boundingBoxMax.x, props.boundingBoxMin.x );
    float yTranslucency = GetBox1DTranslucency( particle.GetPosition().y, props.boundingBoxMax.y, props.boundingBoxMin.y );
    float zTranslucency = GetBox1DTranslucency( particle.GetPosition().z, props.boundingBoxMax.z, props.boundingBoxMin.z );

    return ( xTranslucency + yTranslucency + zTranslucency ) / 3.0f;
}

float ParticleEmitter::GetBox1DTranslucency( float coord, float maxCoord, float minCoord ) {
    float translucency = 1.0f;

    if( coord > maxCoord ) {
        translucency = abs( maxCoord / coord );
    }

    if( coord < minCoord ) {
        translucency = abs( coord / minCoord );
    }

    return 255.0f * translucency;
}

int ParticleEmitter::RGBAToInt( Vector3 color, int alpha ) {
    return D3DCOLOR_ARGB( alpha, (int)color.x, (int)color.y, (int)color.z );
}

void ParticleEmitter::ResurrectParticle( SParticle & p ) {
    if( props.type == PS_BOX ) {
        p.SetPosition( RandomVector3( props.boundingBoxMin, Vector3( props.boundingBoxMax.x, props.boundingBoxMin.y, props.boundingBoxMax.z )  ));
        p.SetSpeed( RandomVector3( props.speedDeviationMin, props.speedDeviationMax ));
    }

    if( props.type == PS_STREAM ) {
        p.SetPosition( Vector3( 0, 0, 0 ) );
        p.SetSpeed( RandomVector3( props.speedDeviationMin, props.speedDeviationMax ));
    }

    p.SetSize( props.pointSize );
    p.SetTranslucency( 255 );
}

Vector3 ParticleEmitter::RandomVector3( Vector3 & min, Vector3 & max ) {
    return Vector3( frandom( min.x, max.x ), frandom( min.y, max.y ), frandom( min.z, max.z ) );
}

bool ParticleEmitter::HasAliveParticles() {
    return aliveParticles > 0;
}

bool ParticleEmitter::IsEnabled() {
    return props.enabled;
}

float ParticleEmitter::GetThickness() {
    return props.particleThickness;
}

void ParticleEmitter::ResurrectParticles() {
    for( size_t i = 0; i < particles.size(); i++ ) {
        ResurrectParticle( particles.at( i ));
    }
}

ParticleEmitter::ParticleEmitter( SceneNode * theParent, int theParticleCount, ParticleSystemProperties creationProps ) {
    base = theParent;
    aliveParticles = theParticleCount; 
    g_device->CreateVertexBuffer( theParticleCount * 4 * sizeof( SParticleVertex ), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_MANAGED, &vertexBuffer, 0 );
    g_device->CreateIndexBuffer( theParticleCount * 2 * sizeof( SParticleFace ), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer, 0 );
    faces = new SParticleFace[ theParticleCount * 2 ];
    vertices = new SParticleVertex[ theParticleCount * 4 ];
	for( int i = 0; i < theParticleCount; i++ ) {
		particles.push_back( SParticle()); 
	}
	props = creationProps;
    firstTimeUpdate = false;
    g_particleEmitters.push_back( this );
    ResurrectParticles();
}

bool ParticleEmitter::ZSorter::operator()( SParticle & p1, SParticle & p2 ) {
    D3DXVECTOR3 viewSpacePos1;
    D3DXVec3TransformCoord( &viewSpacePos1, &D3DXVECTOR3( p1.GetPosition().x, p1.GetPosition().y, p1.GetPosition().z ), &worldView );

    D3DXVECTOR3 viewSpacePos2;
    D3DXVec3TransformCoord( &viewSpacePos2, &D3DXVECTOR3( p2.GetPosition().x, p2.GetPosition().y, p2.GetPosition().z ), &worldView );

    return viewSpacePos1.z < viewSpacePos2.z;
}

void ParticleEmitter::SParticle::Move() {
    position = position + speed;
}

float ParticleEmitter::SParticle::GetSize() {
    return size;
}

void ParticleEmitter::SParticle::SetSize( float theSize ) {
    size = theSize;
}

float ParticleEmitter::SParticle::GetTranslucency() {
    return translucency;
}

void ParticleEmitter::SParticle::SetTranslucency( float theTranslucency ) {
    translucency = theTranslucency;
}

Vector3 ParticleEmitter::SParticle::GetColor() const {
    return color;
}

void ParticleEmitter::SParticle::SetColor( const Vector3 & theColor ) {
    color = theColor;
}

Vector3 ParticleEmitter::SParticle::GetSpeed() const {
    return speed;
}

void ParticleEmitter::SParticle::SetSpeed( const Vector3 & theSpeed ) {
    speed = theSpeed;
}

Vector3 ParticleEmitter::SParticle::GetPosition() const {
    return position;
}

void ParticleEmitter::SParticle::SetPosition( const btVector3 & thePosition ) {
    position.x = thePosition.x();
    position.y = thePosition.y();
    position.z = thePosition.z();
}

void ParticleEmitter::SParticle::SetPosition( const Vector3 & thePosition ) {
    position = thePosition;
}

ParticleEmitter::SParticle::SParticle() {
    SetPosition( Vector3( 0, 0, 0 ) );
    SetSpeed( Vector3( 0, 0, 0 ) );
    SetColor( Vector3( 255, 255, 255 ));
    SetTranslucency( 255 );
    SetSize( 1.0f );
}

ParticleEmitter::SParticle::SParticle( const Vector3 & thePosition, const Vector3 & theSpeed, const Vector3 & theColor, float theTranslucency, float theSize ) {
    SetPosition( thePosition );
    SetSpeed( theSpeed );
    SetColor( theColor );
    SetTranslucency( theTranslucency );
    SetSize( theSize );
}

ParticleEmitter::SParticleFace::SParticleFace() {
    v1 = 0;
    v2 = 0;
    v3 = 0;
}

ParticleEmitter::SParticleFace::SParticleFace( short theFirstVertex, short theSecondVertex, short theThirdVertex ) {
    v1 = theFirstVertex;
    v2 = theSecondVertex;
    v3 = theThirdVertex;
}

ParticleEmitter::SParticleVertex::SParticleVertex() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;

    tx = 0.0f;
    ty = 0.0f;

    color = 0xFFFFFFFF;
}

ParticleEmitter::SParticleVertex::SParticleVertex( Vector3 thePosition, float theTextureCoordX, float theTextureCoordY, int theColor ) {
    x = thePosition.x;
    y = thePosition.y;
    z = thePosition.z;

    tx = theTextureCoordX;
    ty = theTextureCoordY;

    color = theColor;
}

ParticleEmitter::SParticleVertex::SParticleVertex( float theX, float theY, float theZ, float theTextureCoordX, float theTextureCoordY, int theColor ) {
    x = theX;
    y = theY;
    z = theZ;

    tx = theTextureCoordX;
    ty = theTextureCoordY;

    color = theColor;
}

////////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////////

NodeHandle CreateParticleSystem( int particleNum, ParticleSystemProperties creationProps ) {
	SceneNode * node = new SceneNode;
	node->particleEmitter = new ParticleEmitter( node, particleNum, creationProps );
	return SceneNode::HandleFromPointer( node );
}

int GetParticleSystemAliveParticles( NodeHandle ps ) {
	SceneNode * n = SceneNode::CastHandle( ps );

	if( n->particleEmitter ) {
		return n->particleEmitter->GetAliveParticles();
	}

	return 0;
}

void ResurrectDeadParticles( NodeHandle ps ) {
	SceneNode * n = SceneNode::CastHandle( ps );

	if( n->particleEmitter ) {
		return n->particleEmitter->ResurrectParticles();
	}
};

ParticleSystemProperties * GetParticleSystemProperties( NodeHandle ps ) {
	SceneNode * n = SceneNode::CastHandle( ps );

	if( n->particleEmitter ) {
		return &n->particleEmitter->props;
	}

	return 0;
}