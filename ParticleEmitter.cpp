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
    CheckDXErrorFatal( g_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, aliveParticles * 4, 0, aliveParticles * 2 ));
}

void ParticleEmitter::Bind() {
	Texture * texPtr = (Texture *)props.texture.pointer;
	if( texPtr ) {
		texPtr->Bind( 0 );
	}
    CheckDXErrorFatal( g_device->SetStreamSource( 0, vertexBuffer, 0, sizeof( SParticleVertex )));
    CheckDXErrorFatal( g_device->SetIndices( indexBuffer ));
}

void ParticleEmitter::Update() {
    if( !firstTimeUpdate ) {
        ResurrectParticles();
        firstTimeUpdate = true;
    }

    aliveParticles = 0;

    base->globalTransform.getBasis().setEulerYPR( 0, 0, 0 );
    GetD3DMatrixFromBulletTransform( base->globalTransform, world );

    D3DXMATRIX view = g_camera->view;

    Vector3 rightVect = Vector3( view._11, view._21, view._31 ).Normalize();
    Vector3 upVect = Vector3( view._12, view._22, view._32 ).Normalize();

    Vector3 leftTop = upVect - rightVect;
    Vector3 rightTop = upVect + rightVect;
    Vector3 rightBottom = rightVect - upVect;
    Vector3 leftBottom = -( rightVect + upVect );

    int vertexNum = 0, faceNum = 0;

    D3DXMatrixMultiply( &zSorter.worldView, &world, &view );
    sort( particles.begin(), particles.end(), zSorter );

    for( auto & p : particles ) {
        p.position += p.speed;

		float insideCoeff = 1.0f;
		if( props.type == PS_BOX ) {
			insideCoeff = ( p.position - Vector3( base->globalTransform.getOrigin().m_floats )).Length2() / ( props.boundingBoxMax - props.boundingBoxMin ).Length2();
		} else {
			insideCoeff = ( p.position - Vector3( base->globalTransform.getOrigin().m_floats )).Length2() / props.boundingRadius;
		}
		if( insideCoeff > 1.0f ) {
			insideCoeff = 1.0f;
		}
		p.color = props.colorBegin.Lerp( props.colorEnd, insideCoeff );
		p.translucency = 255.0f * ( 1.0f - insideCoeff );
        p.size += props.scaleFactor;

        if( p.translucency <= 1.0f  ) {
            if( props.autoResurrectDeadParticles ) {
                ResurrectParticle( p );
            }
        } else {
            vertices[ vertexNum ] = SParticleVertex( p.position + leftTop * p.size, 0.0, 0.0, RGBAToInt( p.color, p.translucency ));
            vertices[ vertexNum + 1 ] = SParticleVertex( p.position + rightTop * p.size, 1.0, 0.0, RGBAToInt( p.color, p.translucency ));
            vertices[ vertexNum + 2 ] = SParticleVertex( p.position + rightBottom * p.size, 1.0, 1.0, RGBAToInt( p.color, p.translucency ));
            vertices[ vertexNum + 3 ] = SParticleVertex( p.position + leftBottom * p.size, 0.0, 1.0, RGBAToInt( p.color, p.translucency ));

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

int ParticleEmitter::RGBAToInt( Vector3 color, int alpha ) {
    return D3DCOLOR_ARGB( alpha, (int)color.x, (int)color.y, (int)color.z );
}

void ParticleEmitter::ResurrectParticle( SParticle & p ) {
    if( props.type == PS_BOX ) {
        p.position = RandomVector3( props.boundingBoxMin, props.boundingBoxMax );
		p.speed = RandomVector3( props.speedDeviationMin, props.speedDeviationMax );
    } else if( props.type == PS_STREAM ) {
        p.position = Vector3( 0, 0, 0 );
        p.speed = RandomVector3( props.speedDeviationMin, props.speedDeviationMax );
    }

    p.size = props.pointSize;
    p.translucency = 255;
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
    for( auto & particle : particles ) {
        ResurrectParticle( particle );
    }
}

ParticleEmitter::ParticleEmitter( SceneNode * theParent, int theParticleCount, ParticleSystemProperties creationProps ) {
    base = theParent;
    aliveParticles = theParticleCount; 
    CheckDXErrorFatal( g_device->CreateVertexBuffer( theParticleCount * 4 * sizeof( SParticleVertex ), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_MANAGED, &vertexBuffer, 0 ));
    CheckDXErrorFatal( g_device->CreateIndexBuffer( theParticleCount * 2 * sizeof( SParticleFace ), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer, 0 ));
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
    D3DXVec3TransformCoord( &viewSpacePos1, &D3DXVECTOR3( p1.position.x, p1.position.y, p1.position.z ), &worldView );
    D3DXVECTOR3 viewSpacePos2;
    D3DXVec3TransformCoord( &viewSpacePos2, &D3DXVECTOR3( p2.position.x, p2.position.y, p2.position.z ), &worldView );
    return viewSpacePos1.z < viewSpacePos2.z;
}

ParticleEmitter::SParticle::SParticle() {
    position = Vector3( 0, 0, 0 );
    speed = Vector3( 0, 0, 0 );
    color = Vector3( 255, 255, 255 );
    translucency = 255;
    size = 1.0f;
}

ParticleEmitter::SParticle::SParticle( const Vector3 & thePosition, const Vector3 & theSpeed, const Vector3 & theColor, float theTranslucency, float theSize ) {
    position = thePosition;
    speed = theSpeed;
    color = theColor;
    translucency = theTranslucency;
    size = theSize;
}

ParticleEmitter::SParticleFace::SParticleFace() {
    v1 = v2 = v3 = 0;
}

ParticleEmitter::SParticleFace::SParticleFace( short theFirstVertex, short theSecondVertex, short theThirdVertex ) {
    v1 = theFirstVertex;
    v2 = theSecondVertex;
    v3 = theThirdVertex;
}

ParticleEmitter::SParticleVertex::SParticleVertex() {
    x = y = z = tx = ty = 0.0f;
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

////////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////////

/*
===============
CreateParticleSystem
===============
*/
NodeHandle CreateParticleSystem( int particleNum, ParticleSystemProperties creationProps ) {
	SceneNode * node = new SceneNode;
	node->particleEmitter = new ParticleEmitter( node, particleNum, creationProps );
	return SceneNode::HandleFromPointer( node );
}

/*
 ===============
 GetParticleSystemAliveParticles
 ===============
*/
int GetParticleSystemAliveParticles( NodeHandle ps ) {
	SceneNode * n = SceneNode::CastHandle( ps );

	if( n->particleEmitter ) {
		return n->particleEmitter->aliveParticles;
	}

	return 0;
}

/*
===============
ResurrectDeadParticles
===============
*/
void ResurrectDeadParticles( NodeHandle ps ) {
	SceneNode * n = SceneNode::CastHandle( ps );

	if( n->particleEmitter ) {
		return n->particleEmitter->ResurrectParticles();
	}
};

/*
===============
GetParticleSystemProperties
===============
*/
ParticleSystemProperties * GetParticleSystemProperties( NodeHandle ps ) {
	SceneNode * n = SceneNode::CastHandle( ps );

	if( n->particleEmitter ) {
		return &n->particleEmitter->props;
	}

	return 0;
}