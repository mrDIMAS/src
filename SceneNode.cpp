#include "Precompiled.h"
#include "Physics.h"

#include "FastReader.h"
#include "ParticleEmitter.h"
#include "Light.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Octree.h"
#include "Texture.h"
#include "Vertex.h"
#include "Engine.h"

vector< SceneNode* > SceneNode::msNodeList;

SceneNode * SceneNode::CastHandle( ruSceneNode handle ) {
	if( !ruIsNodeHandleValid( handle )) {
		throw std::runtime_error( "Invalid handle passed!" );
	}	
    return reinterpret_cast< SceneNode *>( handle.pointer );
}

ruSceneNode SceneNode::HandleFromPointer( SceneNode * ptr ) {
    ruSceneNode handle;
    handle.pointer = ptr;
    return handle;
}

void SceneNode::EraseUnusedNodes() {
    for( auto node : SceneNode::msNodeList ) {
		auto begin = node->mChildList.begin();
		auto end = node->mChildList.end();
        for( auto iter = begin; iter != end;  ) {
            if( *iter == 0 ) {
                node->mChildList.erase( iter );
            } else {
                ++iter;
            }
		}
	}
}



bool SceneNode::IsRenderable() {
    return IsVisible();
}

void SceneNode::AutoName() {
	mName = StringBuilder( "Unnamed" ) << SceneNode::msNodeList.size();
}

SceneNode::SceneNode( ) : mStatic( false ),  mInFrustum( false ), mParent( nullptr ),mTotalFrameCount( 0 ),
						  mIsSkinned( false ), mVisible( true ), mScene( nullptr ), mContactCount( 0 ),
						  mFrozen( false ), mIsBone( false ), mDepthHack( 0.0f ), particleEmitter( nullptr ),
						  mAlbedo( 0.0f ), mCurrentAnimation( nullptr )  {
	AutoName();    
    mLocalTransform = btTransform( btQuaternion( 0, 0, 0 ), btVector3( 0, 0, 0 ));
    mGlobalTransform = mLocalTransform;
    SceneNode::msNodeList.push_back( this );
}

SceneNode::SceneNode( const SceneNode & source ) {
	AutoName();
	mInFrustum = source.mInFrustum;
	mParent = nullptr;
	mTotalFrameCount = 0;
	mIsSkinned = false;
	mVisible = true;
	mScene = nullptr;
	mLocalTransform = source.mLocalTransform;
	mGlobalTransform = mLocalTransform;
	mContactCount = source.mContactCount;
	mFrozen = source.mFrozen;
	mDepthHack = source.mDepthHack;
	particleEmitter = nullptr;
	mAlbedo = source.mAlbedo;
	mCurrentAnimation = nullptr;

	// copy surfaces
	for( auto pMesh : source.mMeshList ) {
		pMesh->LinkTo( this );
		mMeshList.push_back( pMesh );
	}
	
	// create body
	switch( source.GetBodyType() ) {
	case BodyType::Box:
		SetBoxBody();
		break;
	case BodyType::Convex:
		SetConvexBody();
		break;
	case BodyType::Sphere:
		SetSphereBody();
		break;
	case BodyType::Trimesh:
		SetTrimeshBody();
		break;
	}

	if( mFrozen ) {
		Freeze();
	}

	// copy childs
	for( auto pChild : source.mChildList ) {
		SceneNode * pNewChild = new SceneNode( *pChild );
		pNewChild->mParent = this;
		mChildList.push_back( pNewChild );
	}

	SceneNode::msNodeList.push_back( this );
}

SceneNode::~SceneNode() {
	mHitSound.Free();
	mIdleSound.Free();

	for( auto sound : mSoundList ) {
		sound.Free();
	}

	for( auto pMesh : mMeshList ) {
		pMesh->Unlink( this );
	}

	for( auto child : mChildList ) {
		if( child ) {
			delete child;
		}
	}

	for( auto theNode : SceneNode::msNodeList ) {
		for( size_t i = 0; i < theNode->mChildList.size(); i++ ) {
			if( theNode->mChildList[i] == this ) {
				theNode->mChildList[i] = nullptr;
			}
		}
	}

	for( auto keyframe : mKeyframeList ) {
		delete keyframe;
	}

	for( auto trimesh : mTrimeshList ) {
		delete trimesh;
	}

	for( auto body : mBodyList ) {
		if( body->getCollisionShape() ) {
			delete body->getCollisionShape();
		}

		if( body->getMotionState() ) {
			delete body->getMotionState();
		}

		Physics::mpDynamicsWorld->removeRigidBody( body );

		delete body;
	}

	if( particleEmitter ) {
		delete particleEmitter;
	}

	SceneNode::msNodeList.erase( find( SceneNode::msNodeList.begin(), SceneNode::msNodeList.end(), this ));
}

void SceneNode::SetConvexBody() {
    if( mMeshList.size() == 0 ) {
        return;
    }

    btConvexHullShape * convex = new btConvexHullShape();

    for( auto mesh : mMeshList ) {
        for( auto & vertex : mesh->mVertices ) {
            convex->addPoint ( btVector3( vertex.mPosition.x, vertex.mPosition.y, vertex.mPosition.z ));
        }
    }

    btVector3 inertia ( 0.0f, 0.0f, 0.0f );
    convex->calculateLocalInertia ( 1, inertia );
    SetBody( new btRigidBody ( 1, ( btMotionState * ) ( new btDefaultMotionState() ), ( btCollisionShape * ) ( convex ), inertia ));
}



void SceneNode::SetCapsuleBody( float height, float radius ) {
    btCollisionShape * shape = new btCapsuleShape ( radius, height );
    btVector3 inertia;
    shape->calculateLocalInertia ( 1, inertia );
    SetBody( new btRigidBody ( 1, ( btMotionState * ) ( new btDefaultMotionState() ), shape, inertia ));
}

void SceneNode::SetBoxBody( ) {
    ruVector3 halfExtents = ( GetAABBMax() - GetAABBMin() ) / 2.0f;
    btCollisionShape * shape = new btBoxShape( btVector3( halfExtents.x, halfExtents.y, halfExtents.z ));
    btVector3 inertia;
    shape->calculateLocalInertia ( 1, inertia );
    SetBody( new btRigidBody ( 1, ( btMotionState * ) ( new btDefaultMotionState() ), shape, inertia ));
}

void SceneNode::SetCylinderBody( ) {
    ruVector3 halfExtents = ( GetAABBMax() - GetAABBMin() ) / 2.0f;
    btCollisionShape * shape = new btCylinderShape( btVector3( halfExtents.x, halfExtents.y, halfExtents.z ) );
    btVector3 inertia;
    shape->calculateLocalInertia ( 1, inertia );
    SetBody( new btRigidBody ( 1, ( btMotionState * ) ( new btDefaultMotionState() ), shape, inertia ));
}

void SceneNode::SetSphereBody( ) {
    float radius = ( GetAABBMax() - GetAABBMin() ).Length() / 2.0f;
    btCollisionShape * shape = new btSphereShape( radius );
    btVector3 inertia;
    shape->calculateLocalInertia ( 1, inertia );
    SetBody( new btRigidBody ( 1, ( btMotionState * ) ( new btDefaultMotionState() ), shape, inertia ));
}

void SceneNode::SetAngularFactor( ruVector3 fact ) {
    for( auto body : mBodyList ) {
        body->setAngularFactor( btVector3( fact.x, fact.y, fact.z ) );
    }
}

void SceneNode::SetTrimeshBody() {
    if( mMeshList.size() ) {
		int meshNum = 0;
		mStatic = true;
		for ( auto mesh : mMeshList ) {
			if( mesh->mTriangles.size() ) {
				btTriangleMesh * trimesh = new btTriangleMesh();
				for( auto triangle : mesh->mTriangles ) {
					ruVector3 & a = mesh->mVertices[ triangle.mA ].mPosition;
					ruVector3 & b = mesh->mVertices[ triangle.mB ].mPosition;
					ruVector3 & c = mesh->mVertices[ triangle.mC ].mPosition;
					trimesh->addTriangle ( btVector3( a.x, a.y, a.z ), btVector3( b.x, b.y, b.z ), btVector3( c.x, c.y, c.z ), false );
				};			
				btMotionState * motionState = new btDefaultMotionState();
				btCollisionShape * shape = new btBvhTriangleMeshShape( trimesh, true, true );
				btRigidBody * body = new btRigidBody( 0.0f, motionState, shape );
				body->setWorldTransform ( mGlobalTransform );
				body->setFriction(1.0f);
				body->setUserPointer( this );
				body->setUserIndex( meshNum );
				body->setRestitution( 0.0f );
				body->setDeactivationTime( 0.1f );
				body->setSleepingThresholds( 1.0f, 1.0f );
				body->getCollisionShape()->setMargin( 0.02 );
				mBodyList.push_back( body );
				Physics::mpDynamicsWorld->addRigidBody ( body );
				mTrimeshList.push_back( trimesh );
			}
			meshNum++;
		}
	}
}

void SceneNode::EraseChild( const SceneNode * child ) {
    auto childIterator = find( mChildList.begin(), mChildList.end(), child );

    if( childIterator != mChildList.end() ) {
        mChildList.erase( childIterator );
    }
}

void SceneNode::AttachTo( SceneNode * newParent ) {
    if( newParent ) {
		if( mParent != newParent ) {
			mParent = newParent;
			newParent->mChildList.push_back( this );
		} 
    } else { // Detach
		if( mParent ) {
			mParent->mChildList.erase( find( mParent->mChildList.begin(), mParent->mChildList.end(), this ));
			mParent = nullptr;
		}
	}
}

btTransform & SceneNode::CalculateGlobalTransform() {
    if( mBodyList.size() ) {
        if( mParent ) {
            if( mFrozen ) { // only frozen bodies can be parented
                mGlobalTransform = mParent->CalculateGlobalTransform() * mLocalTransform;
                mBodyList[0]->setWorldTransform( mGlobalTransform );
                mBodyList[0]->setLinearVelocity( btVector3( 0, 0, 0 ));
                mBodyList[0]->setAngularVelocity( btVector3( 0, 0, 0 ));
            } else {
                mGlobalTransform = mBodyList[0]->getWorldTransform();
            }
        } else { // dont has parent
            mGlobalTransform = mBodyList[0]->getWorldTransform();
        }
    } else { // dont has body
        if( mParent ) {
            mGlobalTransform = mParent->CalculateGlobalTransform() * mLocalTransform;
        } else {
            mGlobalTransform = mLocalTransform;
        }
    }

    return mGlobalTransform;
}

ruVector3 SceneNode::GetAABBMin() {
    ruVector3 min = ruVector3( FLT_MAX, FLT_MAX, FLT_MAX );

    for( auto mesh : mMeshList ) {
        AABB aabb = mesh->mAABB;
        if( aabb.mMin.x < min.x ) {
            min.x = aabb.mMin.x;
        }
        if( aabb.mMin.y < min.y ) {
            min.y = aabb.mMin.y;
        }
        if( aabb.mMin.z < min.z ) {
            min.z = aabb.mMin.z;
        }
    }
    return min;
}

ruVector3 SceneNode::GetAABBMax() {
    ruVector3 max = ruVector3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

    for( auto mesh : mMeshList ) {
        AABB aabb = mesh->mAABB;

        if( aabb.mMax.x > max.x ) {
            max.x = aabb.mMax.x;
        }
        if( aabb.mMax.y > max.y ) {
            max.y = aabb.mMax.y;
        }
        if( aabb.mMax.z > max.z ) {
            max.z = aabb.mMax.z;
        }
    }
    return max;
}

SceneNode * SceneNode::Find( SceneNode * parent, string childName ) {
    for( auto child : parent->mChildList )
        if( child->mName == childName ) {
            return child;
        }
    return nullptr;
}

SceneNode * SceneNode::LoadScene( const string & file ) {
    FastReader reader;

    if ( !reader.ReadFile( file ) ) {
        MessageBoxA( 0, (StringBuilder( "Unable to load" ) << file << "scene!").ToCStr(), 0, MB_OK | MB_ICONERROR );
        exit( -1 );
        return nullptr;
    }

    int numObjects = reader.GetInteger();
    int numMeshes = reader.GetInteger();
    int numLights = reader.GetInteger();
    int framesCount = reader.GetInteger();

    SceneNode * scene = new SceneNode;

    scene->mTotalFrameCount = framesCount;

    for ( int meshObjectNum = 0; meshObjectNum < numMeshes; meshObjectNum++ ) {
		SceneNode * node = new SceneNode;

        node->mLocalTransform.setOrigin( reader.GetVector() );
        node->mLocalTransform.setRotation( reader.GetQuaternion() );
        node->mGlobalTransform = node->mLocalTransform;

        int hasAnimation = reader.GetInteger();
        int isSkinned = reader.GetInteger();
        int meshCount = reader.GetInteger();
        int keyframeCount = reader.GetInteger();
        node->mIsSkinned = isSkinned;
        ParseString( reader.GetString(), node->mProperties );
        node->mName = reader.GetString();
        for( int i = 0; i < keyframeCount; i++ ) {
            btTransform * keyframe = new btTransform;
            keyframe->setOrigin( reader.GetVector());
            keyframe->setRotation( reader.GetQuaternion());
            node->mKeyframeList.push_back( keyframe );
        }

        if( keyframeCount ) {
            node->mLocalTransform = *node->mKeyframeList[ 0 ];
        }

        node->mTotalFrameCount = framesCount - 1; // - 1 because numeration started from zero

        for( int i = 0; i < meshCount; i++ ) {
            Mesh * mesh = new Mesh;
			mesh->LinkTo( node );

            int vertexCount = reader.GetInteger();
            int indexCount = reader.GetInteger();

            ruVector3 aabbMin = reader.GetBareVector();
            ruVector3 aabbMax = reader.GetBareVector();
            ruVector3 aabbCenter = reader.GetBareVector(); // odd
            float aabbRadius = reader.GetFloat(); // odd
            //mesh->aabb = AABB( aabbMin, aabbMax );

            string diffuse = reader.GetString();
            string normal = reader.GetString();
            mesh->mOpacity = reader.GetFloat() / 100.0f;

            mesh->mVertices.reserve( vertexCount );
            for( int vertexNum = 0; vertexNum < vertexCount; vertexNum++ ) {
                Vertex v;

                v.mPosition = reader.GetBareVector();
                v.mNormal = reader.GetBareVector();
                v.mTexCoord = reader.GetBareVector2();
                ruVector2 tc2 = reader.GetBareVector2(); // just read secondary texcoords, but don't add it to the mesh. need to compatibility
                v.mTangent = reader.GetBareVector();

                mesh->mVertices.push_back( v );
            }

            mesh->mAABB = AABB( mesh->mVertices );

            mesh->mTriangles.reserve( indexCount );

            for( int indexNum = 0; indexNum < indexCount; indexNum += 3 ) {
                unsigned short a = reader.GetShort();
                unsigned short b = reader.GetShort();
                unsigned short c = reader.GetShort();

                mesh->mTriangles.push_back( Mesh::Triangle( a, b, c ));
            }

            mesh->mDiffuseTexture = Texture::Require( Engine::Instance().GetTextureStoragePath() + diffuse );
			if( mesh->mOpacity > 0.95f ) {
				mesh->mNormalTexture = Texture::Require( Engine::Instance().GetTextureStoragePath() + normal );

				string height = diffuse.substr( 0, diffuse.find_first_of( '.' )) + "_height" + diffuse.substr( diffuse.find_first_of( '.' ));
				mesh->mHeightTexture = Texture::Require( Engine::Instance().GetTextureStoragePath() + height );
			}
            node->mMeshList.push_back( mesh );

            if( node->mIsSkinned ) {
                for( int k = 0; k < vertexCount; k++ ) {
                    Mesh::Weight w;

                    w.mBoneCount = reader.GetInteger();

                    for( int j = 0; j < w.mBoneCount; j++ ) {
                        w.mBone[ j ].mID = reader.GetInteger();
                        w.mBone[ j ].mWeight = reader.GetFloat();
                    }

                    mesh->mWeightTable.push_back( w );
                }
            }

            if( vertexCount != 0 ) {
                mesh->UpdateBuffers();
                Mesh::Register( mesh );
            }
        }
		        
		node->mParent = scene;
		scene->mChildList.push_back( node );
		
        node->mScene = scene;
        node->ApplyProperties();
    }

    for( int lightObjectNum = 0; lightObjectNum < numLights; lightObjectNum++ ) {
        string name = reader.GetString();
        int type = reader.GetInteger();
        Light * light = new Light( type );
        light->mName = name;
        light->SetColor( reader.GetBareVector());
        light->SetRadius( reader.GetFloat());
        light->brightness = reader.GetFloat();
        light->mLocalTransform.setOrigin( reader.GetVector());
        light->mScene = scene;
        light->mParent = scene;
        scene->mChildList.push_back( light );
        if( type == LT_SPOT ) {
            float in = reader.GetFloat();
            float out = reader.GetFloat();
            light->SetConeAngles( in, out );
            light->mLocalTransform.setRotation( reader.GetQuaternion());
        }
    }

    for( auto child : scene->mChildList ) {
        string objectName = reader.GetString();
        string parentName = reader.GetString();

        SceneNode * object = Find( scene, objectName );
        SceneNode * parent = Find( scene, parentName );

        if( parent ) {
            parent->mChildList.push_back( object );
            object->mParent = parent;			
        }
    }

	for( auto node : scene->mChildList ) {
        node->mInvBoneBindTransform = node->CalculateGlobalTransform().inverse();
    }

    return scene;
}

void SceneNode::PerformAnimation() {
    if ( mIsSkinned ) {
        int vertexNumber = 0;
				
		SceneNode * parent = mParent; // HAX!
        mParent = nullptr; // HAX!

        for( auto mesh : mMeshList ) {
            mesh->mSkinVertices = mesh->mVertices;
            for( auto & vertex : mesh->mVertices ) {
                Mesh::Weight & weight = mesh->mWeightTable[ vertexNumber ];

                btVector3 initialPosition( vertex.mPosition.x, vertex.mPosition.y, vertex.mPosition.z );
                btVector3 initialNormal( vertex.mNormal.x, vertex.mNormal.y, vertex.mNormal.z );
                btVector3 initialTangent( vertex.mTangent.x, vertex.mTangent.y, vertex.mTangent.z );
                btVector3 newPosition( 0, 0, 0 );
                btVector3 newNormal( 0, 0, 0 );
                btVector3 newTangent( 0, 0, 0 );

                for( int j = 0; j < weight.mBoneCount; j++ ) {
                    Mesh::Bone & bone = weight.mBone[ j ];
                    SceneNode * boneNode = mScene->mChildList[ bone.mID ];
										
					boneNode->mIsBone = true; // HAX!

                    btTransform transform = ( boneNode->mGlobalTransform * boneNode->mInvBoneBindTransform ) * CalculateGlobalTransform();
                    newPosition += transform * initialPosition * bone.mWeight;
                    newNormal += transform.getBasis() * ( initialNormal * bone.mWeight );
                    newTangent += transform.getBasis() * ( initialTangent * bone.mWeight );
					
                }
				//newNormal = initialNormal;
				//newTangent = initialTangent;

                vertex.mPosition = ruVector3( newPosition.m_floats );
                vertex.mNormal = ruVector3( newNormal.m_floats );
                vertex.mTangent = ruVector3( newTangent.m_floats );

                vertexNumber++;
            }
            mesh->UpdateBuffers();
            mesh->mVertices = mesh->mSkinVertices;
        } 
		mParent = parent; // HAX!
    } else {
        if( mCurrentAnimation ) {
			if ( mKeyframeList.size() ) {
				btTransform * currentFrameTransform = mKeyframeList[ mCurrentAnimation->currentFrame ];
				btTransform * nextFrameTransform = mKeyframeList[ mCurrentAnimation->nextFrame ];
				mLocalTransform.setRotation( currentFrameTransform->getRotation().slerp( nextFrameTransform->getRotation(), mCurrentAnimation->interpolator ));
				mLocalTransform.setOrigin( currentFrameTransform->getOrigin().lerp( nextFrameTransform->getOrigin(), mCurrentAnimation->interpolator ));
			}            
        }
    }
}

void SceneNode::Freeze() {
    mFrozen = true;
	for( auto body : mBodyList ) {
		body->setAngularFactor( 0 );
		body->setLinearFactor( btVector3( 0, 0, 0 ));
		body->setAngularVelocity( btVector3( 0, 0, 0 ));
		body->setLinearVelocity( btVector3( 0, 0, 0 ));
		body->setGravity( btVector3( 0, 0, 0 ));
	}
}

void SceneNode::Unfreeze() {
    mFrozen = false;
	for( auto body : mBodyList ) {
		body->activate(true);
		body->setAngularFactor( 1 );
		body->setLinearFactor( btVector3( 1, 1, 1 ));
		body->setGravity( Physics::mpDynamicsWorld->getGravity() );
	}
}

bool SceneNode::IsVisible() {
    bool trulyVisible = mVisible;

    if( mParent ) {
        trulyVisible &= mParent->IsVisible();
    }

    return trulyVisible;
}

void SceneNode::Hide() {
    mVisible = false;
}

void SceneNode::Show() {
    mVisible = true;
}

std::string SceneNode::GetProperty( string propName ) {
    auto propIter = mProperties.find( propName );

    if( propIter != mProperties.end()) {
        return propIter->second;
    }

    return string( "(Nothing)" );
}

void SceneNode::UpdateSounds() {
    btVector3 pos = mGlobalTransform.getOrigin();

    for( auto sound : mSoundList ) {
        pfSetSoundPosition( sound.pfHandle, pos.x(), pos.y(), pos.z() );
    }

    if( mIdleSound.pfHandle ) {
        pfPlaySound( mIdleSound.pfHandle, true );
    }
}

void SceneNode::UpdateContacts() {
    int numManifolds = Physics::mpDynamicsWorld->getDispatcher()->getNumManifolds();

    for( auto node : SceneNode::msNodeList ) {
        if( node->mBodyList.size() ) {
            node->mContactCount = 0;
        }
	}

    for (int i=0; i < numManifolds; i++) {
        btPersistentManifold* contactManifold = Physics::mpDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        SceneNode * nodeA = static_cast<SceneNode*>( obA->getUserPointer() );
        SceneNode * nodeB = static_cast<SceneNode*>( obB->getUserPointer() );

        if( !nodeA || !nodeB ) {
            continue;
        }

		if( nodeA == nodeB ) { // ???
			continue;
		}

        int numContacts = contactManifold->getNumContacts();

        if( numContacts > BODY_MAX_CONTACTS ) {
            numContacts = BODY_MAX_CONTACTS;
        }

        for (int j = 0 ; j < numContacts; j++ ) {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);

            if( pt.getDistance() < 0.0f ) {
				nodeA->mContactCount++;
				nodeB->mContactCount++;

				int obAIndex = obA->getUserIndex();
                nodeA->mContactList[ j ].normal = ruVector3( pt.m_normalWorldOnB.x(), pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z());
				nodeA->mContactList[ j ].position = ruVector3( pt.m_positionWorldOnA.x(), pt.m_positionWorldOnA.y(), pt.m_positionWorldOnA.z());
				nodeA->mContactList[ j ].impulse = pt.m_appliedImpulse;
				nodeA->mContactList[ j ].body.pointer = nodeB;
				if( obAIndex >= 0 ) {
					if( nodeA->mMeshList[obAIndex]->mDiffuseTexture ) {
						nodeA->mContactList[ j ].textureName = nodeA->mMeshList[obAIndex]->mDiffuseTexture->GetName();
					}
				}

				int obBIndex = obB->getUserIndex();
                nodeB->mContactList[ j ].normal = ruVector3( pt.m_normalWorldOnB.x(), pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z());                
                nodeB->mContactList[ j ].position = ruVector3( pt.m_positionWorldOnB.x(), pt.m_positionWorldOnB.y(), pt.m_positionWorldOnB.z());                
                nodeB->mContactList[ j ].impulse = pt.m_appliedImpulse;				
				nodeB->mContactList[ j ].body.pointer = nodeA;
				if( obBIndex >= 0 ) {
					if( nodeB->mMeshList[obBIndex]->mDiffuseTexture ) {
						nodeB->mContactList[ j ].textureName = nodeB->mMeshList[obBIndex]->mDiffuseTexture->GetName();
					}
				}

                if( pt.m_appliedImpulse > 10.0f ) {
                    if( !nodeA->mFrozen ) {
                        if( nodeA->mHitSound.pfHandle ) {
                            pfPlaySound( nodeA->mHitSound.pfHandle );
                        }
					}
                    if( !nodeB->mFrozen ) {
                        if( nodeB->mHitSound.pfHandle ) {
                            pfPlaySound( nodeB->mHitSound.pfHandle );
                        }
					}
                    float vol = pt.m_appliedImpulse / 20.0f;

                    if( vol > 1.0f ) {
                        vol = 1.0f;
                    }

                    if( nodeA->mHitSound.pfHandle ) {
                        pfSetSoundVolume( nodeA->mHitSound.pfHandle, vol );
                    }
                    if( nodeB->mHitSound.pfHandle ) {
                        pfSetSoundVolume( nodeB->mHitSound.pfHandle, vol );
                    }

                    float pc = pt.m_appliedImpulse / 30.0f;
                    if( pc > 0.7f ) {
                        pc = 0.7f;
                    }

                    float pitch = 0.6f + pc;

                    if( nodeA->mHitSound.pfHandle ) {
                        pfSetSoundPitch( nodeA->mHitSound.pfHandle, pitch );
                    }
                    if( nodeB->mHitSound.pfHandle ) {
                        pfSetSoundPitch( nodeB->mHitSound.pfHandle, pitch );
                    }
                }
            }
        }
    }
}

void SceneNode::SetLinearFactor( ruVector3 lin ) {
    for( auto body : mBodyList ) {
        body->setLinearFactor( btVector3( lin.x, lin.y, lin.z ) );
    }
}

ruVector3 SceneNode::GetPosition() {
    btVector3 pos = mGlobalTransform.getOrigin();
    return ruVector3( pos.x(), pos.y(), pos.z() );
}

int SceneNode::GetContactCount() {
    return mContactCount;
}

ruContact SceneNode::GetContact( int num ) {
    return mContactList[ num ];
}

void SceneNode::ApplyProperties() {
    for( auto prop : mProperties ) {
        string pname = prop.first;
        string value = prop.second;

        if ( pname == "body" ) {
            if ( value == "static" ) {
                SetTrimeshBody();
            }
            if ( value == "convex" ) {
                SetConvexBody();
            }
            if ( value == "box" ) {
                SetBoxBody();
            }
            if ( value == "sphere" ) {
                SetSphereBody();
            }
            if ( value == "cylinder" ) {
                SetCylinderBody();
            }
        };

        if( pname == "albedo" ) {
            mAlbedo = atof( value.c_str() );
        }

        if ( pname == "octree" ) {
            if ( value == "1" ) {
                for( auto mesh : mMeshList ) {
                    if( !mesh->mOctree ) {
                        // mesh->octree = new Octree( mesh, 4096 );
                    }
                }
            }
        }

        if ( pname == "visible" ) {
            mVisible = atoi( value.c_str());
        }

        if ( pname == "frozen" ) {
            int frozen = atoi( value.c_str());

            if( frozen ) {
                Freeze();
            }
        }

        if ( pname == "mass" ) {
			if( !IsStatic() ) {
				SetMass( atof( value.c_str()) );            
			}
		}

        if ( pname == "friction" ) {
            for( auto body : mBodyList ) {
                body->setFriction ( atof( value.c_str()) );
            }
		}

        if ( pname == "hitSound" ) {
            mHitSound = ruSound::Load3D( value.c_str());
            AttachSound( mHitSound );
        };

        if ( pname == "idleSound" ) {
            mIdleSound = ruSound::Load3D( value.c_str());
            AttachSound( mIdleSound );
        };
    };
}


void SceneNode::AttachSound( ruSound sound ) {
    mSoundList.push_back( sound );
}

bool SceneNode::IsNodeInside( SceneNode * node ) {
    if( !node ) {
        return 0;
    }

    btVector3 pos( mGlobalTransform.getOrigin());

    ruVector3 point( pos.x(), pos.y(), pos.z() );

    int result = 0;

    btVector3 n2Pos = node->mGlobalTransform.getOrigin();
    for( auto mesh : node->mMeshList ) {
        AABB aabb = mesh->mAABB;

        aabb.mMax.x += n2Pos.x();
        aabb.mMax.y += n2Pos.y();
        aabb.mMax.z += n2Pos.z();

        aabb.mMin.x += n2Pos.x();
        aabb.mMin.y += n2Pos.y();
        aabb.mMin.z += n2Pos.z();

        if( point.x > aabb.mMin.x && point.x < aabb.mMax.x &&
                point.y > aabb.mMin.y && point.y < aabb.mMax.y &&
                point.z > aabb.mMin.z && point.z < aabb.mMax.z ) {
            result++;
        }
    }

    for( auto childNode : node->mChildList ) {
        result += childNode->IsNodeInside( this );
    }

    return result;
}

SceneNode * SceneNode::GetChild( int i ) {
    return mChildList[i];
}

int SceneNode::GetCountChildren() {
    return mChildList.size();
}

SceneNode * SceneNode::FindByName( const string & name ) {
    for( auto node : SceneNode::msNodeList )
        if( node->mName == name ) {
            return node;
        }
    return nullptr;
}



void SceneNode::SetFriction( float friction ) {
    for( auto body : mBodyList ) {
        body->setFriction( friction );
    }
}

void SceneNode::SetDepthHack( float depthHack ) {
    mDepthHack = depthHack;
    for( size_t i = 0; i < mChildList.size(); i++ ) {
        mChildList[ i ]->SetDepthHack( depthHack );
    }
}

void SceneNode::SetAnisotropicFriction( ruVector3 aniso ) {
    for( auto body : mBodyList ) {
        body->setAnisotropicFriction( btVector3( aniso.x, aniso.y, aniso.z ));
    }
}

void SceneNode::Move( ruVector3 speed ) {
    for( auto body : mBodyList ) {
		body->activate( true );
        body->setLinearVelocity(  btVector3( speed.x, speed.y, speed.z ) );
    };
	mLocalTransform.setOrigin( mLocalTransform.getOrigin() + btVector3( speed.x, speed.y, speed.z ) );
}

void SceneNode::SetVelocity( ruVector3 velocity ) {
    for( auto body : mBodyList ) {
		body->activate( true );
        body->setLinearVelocity( btVector3( velocity.x, velocity.y, velocity.z ) );
    }
}

void SceneNode::SetPosition( ruVector3 position ) {
	mLocalTransform.setOrigin( btVector3( position.x, position.y, position.z ) );
    for( auto body : mBodyList ) {
		body->activate(true);
        body->getWorldTransform().setOrigin( btVector3( position.x, position.y, position.z ) );
    };
	CalculateGlobalTransform();
}

float SceneNode::GetMass() {
    if( mBodyList.size() ) {
        return 1.0f / mBodyList[0]->getInvMass();
    }

    return 0.0f;
}

bool SceneNode::IsFrozen() {
    return mFrozen;
}

void SceneNode::SetRotation( ruQuaternion rotation ) {
    for( auto body : mBodyList ) {
		body->activate( true );
        body->getWorldTransform().getBasis().setRotation( btQuaternion( rotation.x, rotation.y, rotation.z, rotation.w ) );
    }
	mLocalTransform.setRotation( btQuaternion( rotation.x, rotation.y, rotation.z, rotation.w ));
}

void SceneNode::SetBodyLocalScaling( ruVector3 scale ) {
	for( auto body : mBodyList ) {
		body->getCollisionShape()->setLocalScaling( btVector3( scale.x, scale.y, scale.z ));
	}
}

ruVector3 SceneNode::GetLookVector() {
    btVector3 look = mLocalTransform.getBasis().getColumn ( 2 );
    return ruVector3( look.x(), look.y(), look.z() );
}

ruVector3 SceneNode::GetAbsoluteLookVector() {
    btVector3 look = mGlobalTransform.getBasis().getColumn ( 2 );
    return ruVector3( look.x(), look.y(), look.z() );
}

const string & SceneNode::GetName() {
    return mName;
}

ruVector3 SceneNode::GetRightVector() {
    btVector3 right = mLocalTransform.getBasis().getColumn ( 0 );
    return ruVector3( right.x(), right.y(), right.z() );
}

ruVector3 SceneNode::GetUpVector() {
    btVector3 up = mLocalTransform.getBasis().getColumn ( 1 );
    return ruVector3( up.x(), up.y(), up.z() );
}

btTransform & SceneNode::GetGlobalTransform() {
    return mGlobalTransform;
}

ruVector3 SceneNode::GetLocalPosition() {
    btTransform transform = mLocalTransform;

    if( mBodyList.size() && !mFrozen ) {
        transform = mBodyList[0]->getWorldTransform();
    }

    ruVector3 lp;

    lp.x = transform.getOrigin().x();
    lp.y = transform.getOrigin().y();
    lp.z = transform.getOrigin().z();

    return lp;
}

SceneNode * SceneNode::FindInObjectByName( SceneNode * node, const string & name ) {
    if( node->mName == name ) {
        return node;
    }


    for( int i = 0; i < node->mChildList.size(); i++ ) {
        SceneNode * child = node->mChildList[ i ];

        SceneNode * lookup = FindInObjectByName( child, name );

        if( lookup ) {
            return lookup;
        }
    }

    return 0;
}

void SceneNode::SetAngularVelocity( ruVector3 velocity ) {
    for( auto body : mBodyList ) {
        body->setAngularVelocity( btVector3( velocity.x, velocity.y, velocity.z ));
    }
}

ruVector3 SceneNode::GetEulerAngles() {
    float y, p, r;

    mGlobalTransform.getBasis().getEulerYPR( y, p, r );

    y *= 180.0 / 3.14159f;
    p *= 180.0 / 3.14159f;
    r *= 180.0 / 3.14159f;

    return ruVector3( p, y, r );
}

ruQuaternion SceneNode::GetLocalRotation() {
    btTransform transform = mLocalTransform;
    if( mBodyList.size() ) {
        transform = mBodyList[0]->getWorldTransform();
    }
    return ruQuaternion( transform.getRotation().x(), transform.getRotation().y(), transform.getRotation().z(), transform.getRotation().w() );
}

void SceneNode::SetDamping( float linearDamping, float angularDamping ) {
    for( auto body : mBodyList ) {
        body->setDamping( linearDamping, angularDamping );
    }
}

void SceneNode::SetGravity( const ruVector3 & gravity ) {
    btVector3 g( gravity.x, gravity.y, gravity.z );

    for( auto body : mBodyList ) {
        body->setGravity( g );
    }
}

void SceneNode::SetMass( float mass ) {
    for( auto body : mBodyList ) {
        btVector3 inertia;
        body->getCollisionShape()->calculateLocalInertia( mass, inertia );
        body->setMassProps( mass, inertia );
    }
}

void SceneNode::SetBody( btRigidBody * theBody ) {
    theBody->setWorldTransform ( mGlobalTransform );
    theBody->setFriction(1.0f);
    theBody->setUserPointer( this );
	theBody->setUserIndex( mMeshList.size() > 0 ? 0 : -1 );
    theBody->setRestitution( 0.0f );
	theBody->setDeactivationTime( 0.1f );
	theBody->setCcdMotionThreshold( 0.75f );
	theBody->setCcdSweptSphereRadius( 0.2f );
	theBody->setSleepingThresholds( 1.0f, 1.0f );
	theBody->getCollisionShape()->setMargin(0.02);
    Physics::mpDynamicsWorld->addRigidBody ( theBody );
	mBodyList.push_back( theBody );
}

void SceneNode::SetAnimation( ruAnimation * newAnim, bool dontAffectChilds ) {
    mCurrentAnimation = newAnim;
    if( !dontAffectChilds ) {
        for( auto child : mChildList ) {
            child->SetAnimation( newAnim, false );
        }
    }
}

BodyType SceneNode::GetBodyType() const {
	BodyType bodyType = BodyType::None;
	if( mBodyList.size() ) {
		btCollisionShape * shape = mBodyList[0]->getCollisionShape();
		if( dynamic_cast<btSphereShape*>( shape )) {
			bodyType = BodyType::Sphere;
		}
		if( dynamic_cast<btBvhTriangleMeshShape*>( shape )) {
			bodyType = BodyType::Trimesh;
		}
		if( dynamic_cast<btCylinderShape*>( shape )) {
			bodyType = BodyType::Cylinder;
		}
		if( dynamic_cast<btConvexHullShape*>( shape )) {
			bodyType = BodyType::Convex;
		}
		if( dynamic_cast<btBoxShape*>( shape )) {
			bodyType = BodyType::Box;
		}
	}
	return bodyType;
}

ruVector3 SceneNode::GetTotalForce() {
	if( mBodyList.size() ) {
		return ruVector3( mBodyList[0]->getTotalForce().x(), mBodyList[0]->getTotalForce().y(), mBodyList[0]->getTotalForce().z());
	} else {
		return ruVector3( 0.0f, 0.0f, 0.0f );
	}
}

ruAnimation * SceneNode::GetCurrentAnimation() {
	return mCurrentAnimation;
}

ruVector3 SceneNode::GetLinearVelocity() {
	ruVector3 vel;
	if( mBodyList.size() ) {
		vel.x = mBodyList[0]->getLinearVelocity().x();
		vel.y = mBodyList[0]->getLinearVelocity().y();
		vel.z = mBodyList[0]->getLinearVelocity().z();
	}
	return vel;
}

float SceneNode::GetRotationAngle() {
	return mLocalTransform.getRotation().getAngle() * 180.0f / 3.14159;
}

ruVector3 SceneNode::GetRotationAxis() {
	btVector3 axis = mLocalTransform.getRotation().getAxis();
	return ruVector3( axis.x(), axis.y(), axis.z() );
}

void SceneNode::AddTorque( ruVector3 torque ) {
	for( auto body : mBodyList ) {
		body->applyTorque( btVector3( torque.x, torque.y, torque.z ));
	}
}

void SceneNode::AddForceAtPoint( ruVector3 force, ruVector3 point ) {
	for( auto body : mBodyList ) {
		body->applyForce( btVector3( force.x, force.y, force.z ), btVector3( point.x, point.y, point.z ) );
	}
}

void SceneNode::AddForce( ruVector3 force ) {
	for( auto body : mBodyList ) {
		body->applyCentralForce( btVector3( force.x, force.y, force.z ));
	}
}

void SceneNode::OnResetDevice() {

}

void SceneNode::OnLostDevice() {

}

void SceneNode::SetLocalPosition( ruVector3 pos ) {
	mLocalTransform.setOrigin( btVector3( pos.x, pos.y, pos.z ));
	for( auto body : mBodyList ) {
		body->getWorldTransform().setOrigin( btVector3( pos.x, pos.y, pos.z ) );
	}
	CalculateGlobalTransform();
}

void  SceneNode::SetLocalRotation( ruQuaternion rot ) {
	mLocalTransform.setRotation( btQuaternion( rot.x, rot.y, rot.z, rot.w ));
	for( auto body : mBodyList ) {
		body->getWorldTransform().setRotation( btQuaternion( rot.x, rot.y, rot.z, rot.w ) );
	}
	CalculateGlobalTransform( );
}

SceneNode * SceneNode::GetParent() {
	return mParent;
}

bool SceneNode::IsStatic() {
	return mStatic;
}

bool SceneNode::IsDynamic() {
	if( mBodyList.size() ) {
		if( dynamic_cast<btBvhTriangleMeshShape*>( mBodyList[0]->getCollisionShape()) == nullptr ) {
			return true;
		}
	}
	return false;	
}

ruTextureHandle SceneNode::GetTexture( int n )
{
	if( n < 0 || n >= mMeshList.size() ) {
		return ruTextureHandle::Empty();
	} else {
		ruTextureHandle h; h.pointer = mMeshList[n]->GetDiffuseTexture();
		return h;
	}
}

int SceneNode::GetTextureCount()
{
	return mMeshList.size();
}

////////////////////////////////////////////////////
// API Functions
////////////////////////////////////////////////////

ruRutheniumHandle::ruRutheniumHandle() {
    pointer = nullptr;
}

ruRutheniumHandle::~ruRutheniumHandle() {

}

bool ruRutheniumHandle::IsValid() {
    return pointer != nullptr;
}

void ruRutheniumHandle::Invalidate() {
    pointer = nullptr;
}

bool ruSceneNode::operator == ( const ruSceneNode & node ) {
    return pointer == node.pointer;
}

bool ruSceneNode::IsValid() {
	return ruIsNodeHandleValid( *this ) && ruRutheniumHandle::IsValid();
}

void ruCreateOctree( ruSceneNode node, int splitCriteria ) {
	if( SceneNode::CastHandle( node ) == nullptr ) {
		return;
	}
    SceneNode * n = SceneNode::CastHandle( node );

    for( int i = 0; i < n->mMeshList.size(); i++ ) {
        Mesh * mesh = n->mMeshList[ i ];

        if( mesh->mOctree ) {
            delete mesh->mOctree;
        }

        mesh->mOctree = new Octree( mesh, splitCriteria );
    }
}


void ruDeleteOctree( ruSceneNode node ) {
	if( SceneNode::CastHandle( node ) == nullptr ) {
		return;
	}
    SceneNode * n = SceneNode::CastHandle( node );

    for( int i = 0; i < n->mMeshList.size(); i++ ) {
        Mesh * mesh = n->mMeshList[ i ];

        if( mesh->mOctree ) {
            delete mesh->mOctree;
        }
    }
}

bool ruIsNodeHandleValid( ruSceneNode handle ) {
	for( auto pNode : SceneNode::msNodeList ) {
		if( pNode == handle.pointer ) {
			return true;
		}
	}
	return false;
}

int ruGetNodeCountChildren( ruSceneNode node ) {
	if( SceneNode::CastHandle( node ) == nullptr ) {
		return 0;
	}
    return SceneNode::CastHandle( node )->GetCountChildren();
}

bool ruIsNodeHasBody( ruSceneNode node ) {
	if( SceneNode::CastHandle( node ) == nullptr ) {
		return false;
	}
    return SceneNode::CastHandle( node )->mBodyList.size() > 0;
}

int ruGetWorldObjectsCount() {
    return SceneNode::msNodeList.size();
}

ruSceneNode ruGetWorldObject( int i ) {
    ruSceneNode handle;
    handle.pointer = SceneNode::msNodeList[ i ];
    return handle;
}
