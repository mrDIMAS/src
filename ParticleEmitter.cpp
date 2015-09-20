#include "Precompiled.h"
#include "Engine.h"
#include "ParticleEmitter.h"
#include "Camera.h"
#include "Texture.h"
#include "Utility.h"

vector<ParticleEmitter*> ParticleEmitter::msParticleEmitters;

ParticleEmitter::~ParticleEmitter() {
	OnLostDevice();

    delete vertices;
    delete faces;

    ParticleEmitter::msParticleEmitters.erase( find( ParticleEmitter::msParticleEmitters.begin(), ParticleEmitter::msParticleEmitters.end(), this ));
}

void ParticleEmitter::Render() {
    Engine::Instance().RegisterDIP();
    Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, mAliveParticleCount * 4, 0, mAliveParticleCount * 2 );
}

void ParticleEmitter::Bind() {
    Texture * texPtr = (Texture *)props.texture.pointer;
    if( texPtr ) {
        texPtr->Bind( 0 );
    }
    Engine::Instance().GetDevice()->SetStreamSource( 0, vertexBuffer, 0, sizeof( ParticleVertex ));
    Engine::Instance().GetDevice()->SetIndices( indexBuffer );
}

void ParticleEmitter::Update() {
    if( !firstTimeUpdate ) {
        ResurrectParticles();
        firstTimeUpdate = true;
    }

    mAliveParticleCount = 0;

    mOwner->mGlobalTransform.getBasis().setEulerYPR( 0, 0, 0 );
    GetD3DMatrixFromBulletTransform( mOwner->mGlobalTransform, mWorldTransform );

    D3DXMATRIX view = Camera::msCurrentCamera->mView;

    ruVector3 rightVect = ruVector3( view._11, view._21, view._31 ).Normalize();
    ruVector3 upVect = ruVector3( view._12, view._22, view._32 ).Normalize();

    ruVector3 leftTop = upVect - rightVect;
    ruVector3 rightTop = upVect + rightVect;
    ruVector3 rightBottom = rightVect - upVect;
    ruVector3 leftBottom = -( rightVect + upVect );

    int vertexNum = 0, faceNum = 0;

    for( auto & p : particles ) {
        p.position += p.speed;

        float insideCoeff = 1.0f;
        if( props.type == PS_BOX ) {
            insideCoeff = ( p.position ).Length2() / ( props.boundingBoxMax - props.boundingBoxMin ).Length2();
        } else {
            insideCoeff = ( p.position ).Length2() / props.boundingRadius;
        }
        if( insideCoeff > 1.0f ) {
            insideCoeff = 1.0f;
        }
        p.color = props.colorBegin.Lerp( props.colorEnd, insideCoeff );
		float fakeOpacity = 255.0f * ( 1.0f - insideCoeff );
        p.mOpacity = fakeOpacity + props.alphaOffset;
        p.size += props.scaleFactor;

		
        if( p.mOpacity <= 1.0f  ) {
            if( props.autoResurrectDeadParticles ) {
                ResurrectParticle( p );
            }
        } else {
			int color = RGBAToInt( p.color, p.mOpacity );

			short v1 = vertexNum + 1;
			short v2 = vertexNum + 2;
			short v3 = vertexNum + 3;

            vertices[ vertexNum ].pos = p.position + leftTop * p.size;
			vertices[ vertexNum ].tex.x = 0.0f;
			vertices[ vertexNum ].tex.y = 0.0f; 
			vertices[ vertexNum ].color = color;

            vertices[ v1 ].pos = p.position + rightTop * p.size;
			vertices[ v1 ].tex.x = 1.0f;
			vertices[ v1 ].tex.y = 0.0f;
			vertices[ v1 ].color = color;

            vertices[ v2 ].pos = p.position + rightBottom * p.size;
			vertices[ v2 ].tex.x = 1.0f;
			vertices[ v2 ].tex.y = 1.0f;
			vertices[ v2 ].color = color;

            vertices[ v3 ].pos = p.position + leftBottom * p.size;
			vertices[ v3 ].tex.x = 0.0f;
			vertices[ v3 ].tex.y = 1.0f;
			vertices[ v3 ].color = color;

            // indices
            faces[ faceNum ].v1 = vertexNum;
			faces[ faceNum ].v2 = v1;
			faces[ faceNum ].v3 = v3;

			int f1 = faceNum + 1;

            faces[ f1 ].v1 = v1;
			faces[ f1 ].v2 = v2;
			faces[ f1 ].v3 = v3;

            ++mAliveParticleCount;
            vertexNum += 4;
            faceNum += 2;
        }
    }

    void * data = 0;
    vertexBuffer->Lock( 0, 0, &data,  D3DLOCK_DISCARD );
    memcpy( data, vertices, mAliveParticleCount * 4 * sizeof( ParticleVertex ));
    vertexBuffer->Unlock();

    indexBuffer->Lock( 0, 0, &data,  D3DLOCK_DISCARD );
    memcpy( data, faces, mAliveParticleCount * 2 * sizeof( ParticleFace ));
    indexBuffer->Unlock();
}

SceneNode * ParticleEmitter::GetOwner() {
    return mOwner;
}

bool ParticleEmitter::IsLightAffects() {
    return props.useLighting;
}

int ParticleEmitter::RGBAToInt( ruVector3 color, int alpha ) {
    return D3DCOLOR_ARGB( alpha, (int)color.x, (int)color.y, (int)color.z );
}

void ParticleEmitter::ResurrectParticle( Particle & p ) {
    if( props.type == PS_BOX ) {
        p.position = RandomVector3( props.boundingBoxMin, props.boundingBoxMax );
        p.speed = RandomVector3( props.speedDeviationMin, props.speedDeviationMax );
    } else if( props.type == PS_STREAM ) {
        p.position = ruVector3( 0, 0, 0 );
        p.speed = RandomVector3( props.speedDeviationMin, props.speedDeviationMax );
    }

    p.size = props.pointSize;
    p.mOpacity = 255;
}

ruVector3 ParticleEmitter::RandomVector3( ruVector3 & min, ruVector3 & max ) {
    return ruVector3( frandom( min.x, max.x ), frandom( min.y, max.y ), frandom( min.z, max.z ) );
}

bool ParticleEmitter::HasAliveParticles() {
    return mAliveParticleCount > 0;
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

ParticleEmitter::ParticleEmitter( SceneNode * theParent, int theParticleCount, ruParticleSystemProperties creationProps ) {
    mOwner = theParent;
    mAliveParticleCount = theParticleCount;
	mMaxParticleCount = theParticleCount;
	OnResetDevice();
    faces = new ParticleFace[ theParticleCount * 2 ];
    vertices = new ParticleVertex[ theParticleCount * 4 ];
    for( int i = 0; i < theParticleCount; i++ ) {
        particles.push_back( Particle());
    }
    props = creationProps;
    firstTimeUpdate = false;
    ParticleEmitter::msParticleEmitters.push_back( this );
    ResurrectParticles();
}

void ParticleEmitter::OnLostDevice()
{
	vertexBuffer->Release();
	indexBuffer->Release();
}

void ParticleEmitter::OnResetDevice()
{
	Engine::Instance().GetDevice()->CreateVertexBuffer( mMaxParticleCount * 4 * sizeof( ParticleVertex ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vertexBuffer, 0 );
	Engine::Instance().GetDevice()->CreateIndexBuffer( mMaxParticleCount * 2 * sizeof( ParticleFace ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, 0 );
}

ParticleEmitter::Particle::Particle() {
    position = ruVector3( 0, 0, 0 );
    speed = ruVector3( 0, 0, 0 );
    color = ruVector3( 255, 255, 255 );
    mOpacity = 255;
    size = 1.0f;
}

ParticleEmitter::Particle::Particle( const ruVector3 & thePosition, const ruVector3 & theSpeed, const ruVector3 & theColor, float theTranslucency, float theSize ) {
    position = thePosition;
    speed = theSpeed;
    color = theColor;
    mOpacity = theTranslucency;
    size = theSize;
}

////////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////////

/*
===============
CreateParticleSystem
===============
*/
ruSceneNode ruCreateParticleSystem( int particleNum, ruParticleSystemProperties creationProps ) {
    SceneNode * node = new SceneNode;
    node->particleEmitter = new ParticleEmitter( node, particleNum, creationProps );
    return SceneNode::HandleFromPointer( node );
}

/*
 ===============
 GetParticleSystemAliveParticles
 ===============
*/
int ruGetParticleSystemAliveParticles( ruSceneNode ps ) {
    SceneNode * n = SceneNode::CastHandle( ps );

    if( n->particleEmitter ) {
        return n->particleEmitter->mAliveParticleCount;
    }

    return 0;
}

/*
===============
ResurrectDeadParticles
===============
*/
void ruResurrectDeadParticles( ruSceneNode ps ) {
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
ruParticleSystemProperties * ruGetParticleSystemProperties( ruSceneNode ps ) {
    SceneNode * n = SceneNode::CastHandle( ps );

    if( n->particleEmitter ) {
        return &n->particleEmitter->props;
    }

    return 0;
}