/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/
#include "Precompiled.h"
#include "Physics.h"

#include "FastReader.h"
#include "ParticleSystem.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Octree.h"
#include "Texture.h"
#include "Vertex.h"
#include "Engine.h"
#include "SceneFactory.h"

bool SceneNode::IsRenderable() {
    return IsVisible();
}

void SceneNode::AutoName() {
	mName = StringBuilder( "Unnamed" ) << SceneFactory::GetNodeList().size();
}

SceneNode::SceneNode( ) : mStatic( false ),  mInFrustum( false ), mTotalFrameCount( 0 ),
						  mIsSkinned( false ), mVisible( true ), mContactCount( 0 ),
						  mFrozen( false ), mIsBone( false ), mDepthHack( 0.0f ),
						  mAlbedo( 0.0f ), mCurrentAnimation( nullptr ), mBlurAmount( 0.0f )  {
	AutoName();    
    mLocalTransform = btTransform( btQuaternion( 0, 0, 0 ), btVector3( 0, 0, 0 ));
    mGlobalTransform = mLocalTransform;
}

SceneNode::~SceneNode() {
	mHitSound.Free();
	mIdleSound.Free();

	for( auto sound : mSoundList ) {
		sound.Free();
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
}

void SceneNode::SetConvexBody() {
    if( mMeshList.size() == 0 ) {
        return;
    }

    btConvexHullShape * convex = new btConvexHullShape();

    for( auto mesh : mMeshList ) {
        for( auto & vertex : mesh->GetVertices() ) {
            convex->addPoint ( btVector3( vertex.mPosition.x, vertex.mPosition.y, vertex.mPosition.z ));
        }
    }

    btVector3 inertia ( 0.0f, 0.0f, 0.0f );
    convex->calculateLocalInertia ( 1, inertia );
    SetBody( new btRigidBody ( 1, static_cast<btMotionState*>( new btDefaultMotionState() ), static_cast<btCollisionShape*>( convex ), inertia ));
}



void SceneNode::SetCapsuleBody( float height, float radius ) {
    btCollisionShape * shape = new btCapsuleShape ( radius, height );
    btVector3 inertia;
    shape->calculateLocalInertia ( 1, inertia );
    SetBody( new btRigidBody ( 1, static_cast<btMotionState*>( new btDefaultMotionState() ), shape, inertia ));
}

void SceneNode::SetBoxBody( ) {
    ruVector3 halfExtents = ( GetAABBMax() - GetAABBMin() ) / 2.0f;
    btCollisionShape * shape = new btBoxShape( btVector3( halfExtents.x, halfExtents.y, halfExtents.z ));
    btVector3 inertia;
    shape->calculateLocalInertia ( 1, inertia );
    SetBody( new btRigidBody ( 1, static_cast<btMotionState*>( new btDefaultMotionState() ), shape, inertia ));
}

void SceneNode::SetCylinderBody( ) {
    ruVector3 halfExtents = ( GetAABBMax() - GetAABBMin() ) / 2.0f;
    btCollisionShape * shape = new btCylinderShape( btVector3( halfExtents.x, halfExtents.y, halfExtents.z ) );
    btVector3 inertia;
    shape->calculateLocalInertia ( 1, inertia );
    SetBody( new btRigidBody ( 1, static_cast<btMotionState*>( new btDefaultMotionState() ), shape, inertia ));
}

void SceneNode::SetSphereBody( ) {
    float radius = ( GetAABBMax() - GetAABBMin() ).Length() / 2.0f;
    btCollisionShape * shape = new btSphereShape( radius );
    btVector3 inertia;
    shape->calculateLocalInertia ( 1, inertia );
    SetBody( new btRigidBody ( 1, static_cast<btMotionState*>( new btDefaultMotionState() ), shape, inertia ));
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
			if( mesh->GetTriangles().size() ) {
				btTriangleMesh * trimesh = new btTriangleMesh();
				for( auto triangle : mesh->GetTriangles() ) {
					ruVector3 & a = mesh->GetVertices()[ triangle.mA ].mPosition;
					ruVector3 & b = mesh->GetVertices()[ triangle.mB ].mPosition;
					ruVector3 & c = mesh->GetVertices()[ triangle.mC ].mPosition;
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

void SceneNode::Attach( const shared_ptr<ruSceneNode> & parent ) {
	shared_ptr<SceneNode> & pParent = std::dynamic_pointer_cast<SceneNode>( parent );
	//if( mParent.lock() != parent ) {
		mParent = pParent;
		pParent->mChildList.push_back( shared_from_this());
	//}     
}

void SceneNode::Detach() {
	mParent.reset();	
}

btTransform & SceneNode::CalculateGlobalTransform() {	
    if( mBodyList.size() ) {
        if( mParent.use_count() ) {
			shared_ptr<SceneNode> & pParent = mParent.lock();
            if( mFrozen ) { // only frozen bodies can be parented
                mGlobalTransform = pParent->CalculateGlobalTransform() * mLocalTransform;
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
        if( mParent.use_count() ) {
			shared_ptr<SceneNode> & pParent = mParent.lock();
            mGlobalTransform = pParent->CalculateGlobalTransform() * mLocalTransform;
        } else {
            mGlobalTransform = mLocalTransform;
        }
    }

	if( dynamic_cast<Camera*>( this )) {
		int t = 0;
	}
    return mGlobalTransform;
}

ruVector3 SceneNode::GetAABBMin() {
    ruVector3 min = ruVector3( FLT_MAX, FLT_MAX, FLT_MAX );

    for( auto mesh : mMeshList ) {
        AABB aabb = mesh->GetBoundingBox();
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
        AABB aabb = mesh->GetBoundingBox();

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

shared_ptr<SceneNode> SceneNode::Find( const shared_ptr<SceneNode> parent, string childName )
{
    for( auto & child : parent->mChildList ) {
		if( child->mName == childName ) {
			return child;
		}
	}
    return nullptr;
}

shared_ptr<SceneNode> SceneNode::LoadScene( const string & file ) {
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

    shared_ptr<SceneNode> & scene = SceneFactory::CreateSceneNode();

    scene->mTotalFrameCount = framesCount;

    for ( int meshObjectNum = 0; meshObjectNum < numMeshes; meshObjectNum++ ) {
		shared_ptr<SceneNode> & node = SceneFactory::CreateSceneNode();

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
            unique_ptr<btTransform> keyframe = unique_ptr<btTransform>( new btTransform );
            keyframe->setOrigin( reader.GetVector());
            keyframe->setRotation( reader.GetQuaternion());
            node->mKeyframeList.push_back( std::move( keyframe ));
        }

        if( keyframeCount ) {
            node->mLocalTransform = *node->mKeyframeList[ 0 ];
        }

        node->mTotalFrameCount = framesCount - 1; // - 1 because numeration started from zero

        for( int i = 0; i < meshCount; i++ ) {
            shared_ptr<Mesh> & mesh = make_shared<Mesh>();
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
            mesh->SetOpacity( reader.GetFloat() / 100.0f );

            for( int vertexNum = 0; vertexNum < vertexCount; vertexNum++ ) {
                Vertex v;

                v.mPosition = reader.GetBareVector();
                v.mNormal = reader.GetBareVector();
                v.mTexCoord = reader.GetBareVector2();
                ruVector2 tc2 = reader.GetBareVector2(); // just read secondary texcoords, but don't add it to the mesh. need to compatibility
                v.mTangent = reader.GetBareVector();

                mesh->AddVertex( v );
            }

            for( int indexNum = 0; indexNum < indexCount; indexNum += 3 ) {
                unsigned short a = reader.GetShort();
                unsigned short b = reader.GetShort();
                unsigned short c = reader.GetShort();

                mesh->AddTriangle( Mesh::Triangle( a, b, c ));
            }

            mesh->SetDiffuseTexture( Texture::Request( Engine::I().GetTextureStoragePath() + diffuse ));
			if( mesh->GetOpacity() > 0.95f ) {
				mesh->SetNormalTexture( Texture::Request( Engine::I().GetTextureStoragePath() + normal ));

				string height = diffuse.substr( 0, diffuse.find_first_of( '.' )) + "_height" + diffuse.substr( diffuse.find_first_of( '.' ));
				mesh->SetHeightTexture( Texture::Request( Engine::I().GetTextureStoragePath() + height ));
			}
            node->AddMesh( mesh );

            if( node->mIsSkinned ) {
                for( int k = 0; k < vertexCount; k++ ) {
                    Mesh::BoneGroup boneGroup;
                    boneGroup.mBoneCount = reader.GetInteger();
                    for( int j = 0; j < boneGroup.mBoneCount; j++ ) {
						// number of scene node represents bone in the scene
                        boneGroup.mBone[ j ].mID = reader.GetInteger();
                        boneGroup.mBone[ j ].mWeight = reader.GetFloat();
						boneGroup.mBone[ j ].mRealBone = nullptr;
                    }
                    mesh->AddBoneGroup( boneGroup );
                }
            }

            if( vertexCount != 0 ) {
				if( !mesh->IsSkinned() ) {
					mesh->CreateHardwareBuffers();
				}
                Mesh::Register( mesh );
            }
        }
		        
		node->mParent = scene;
		scene->mChildList.push_back( node );
		
        node->mScene = scene;
        node->ApplyProperties();
    }

	// remap bone id's to real scene nodes 
	for( auto & child : scene->mChildList ) {
		if( child->mIsSkinned ) {
			for( auto pMesh : child->mMeshList ) {
				for( auto & w : pMesh->GetBoneTable() ) {
					for( int i = 0; i < w.mBoneCount; ++i ) {
						shared_ptr<SceneNode> & bone = scene->mChildList[ w.mBone[ i ].mID ];
						if( bone ) {
							w.mBone[ i ].mRealBone = pMesh->AddBone( bone );		
						}
					}
				}
				pMesh->CreateHardwareBuffers();
			}
		}
	}

    for( int lightObjectNum = 0; lightObjectNum < numLights; lightObjectNum++ ) {
		shared_ptr<Light> light;
        string name = reader.GetString();
        int type = reader.GetInteger();
		if( type == 0 ) {
			light = SceneFactory::CreatePointLight();
		} else {
			light = SceneFactory::CreateSpotLight();
		}
        light->mName = name;
        light->SetColor( reader.GetBareVector());
        light->SetRange( reader.GetFloat());
        float brightness = reader.GetFloat();
        light->mLocalTransform.setOrigin( reader.GetVector());
        light->mScene = scene;
        light->mParent = scene;
        scene->mChildList.push_back( light );
        if( type > 0 ) {
			shared_ptr<SpotLight> & spot = std::dynamic_pointer_cast<SpotLight>( light );
            float in = reader.GetFloat();
            float out = reader.GetFloat();
            spot->SetConeAngles( in, out );
            light->mLocalTransform.setRotation( reader.GetQuaternion());
        }
    }

    for( auto child : scene->mChildList ) {
        string objectName = reader.GetString();
        string parentName = reader.GetString();

        shared_ptr<SceneNode> & object = Find( scene, objectName );
        shared_ptr<SceneNode> & parent = Find( scene, parentName );

        if( parent ) {
            parent->mChildList.push_back( object );
            object->mParent = parent;			
        }
    }

	for( auto & node : scene->mChildList ) {
		node->mInvBoneBindTransform = node->CalculateGlobalTransform().inverse();		
    }

    return scene;
}

void SceneNode::PerformAnimation() {
    if( !mIsSkinned ) {
		if( mCurrentAnimation ) {
			if ( mKeyframeList.size() ) {
				btTransform * currentFrameTransform = mKeyframeList[ mCurrentAnimation->currentFrame ].get();
				btTransform * nextFrameTransform = mKeyframeList[ mCurrentAnimation->nextFrame ].get();
				mLocalTransform.setRotation( currentFrameTransform->getRotation().slerp( nextFrameTransform->getRotation(), mCurrentAnimation->interpolator ));
				mLocalTransform.setOrigin( currentFrameTransform->getOrigin().lerp( nextFrameTransform->getOrigin(), mCurrentAnimation->interpolator ));
			}            
		}
    };
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

	shared_ptr<SceneNode> & parent = mParent.lock();
    if( parent ) {
        trulyVisible &= parent->IsVisible();
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

	auto & nodes = SceneFactory::GetNodeList(); 
    for( auto & pWeak : nodes ) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if( node ) {
			if( node->mBodyList.size() ) {
				node->mContactCount = 0;
			}
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
				nodeA->mContactList[ j ].body = nodeB;
				if( obAIndex >= 0 ) {
					if( nodeA->mMeshList[obAIndex]->GetDiffuseTexture() ) {
						nodeA->mContactList[ j ].textureName = nodeA->mMeshList[obAIndex]->GetDiffuseTexture()->GetName();
					}
				}

				int obBIndex = obB->getUserIndex();
                nodeB->mContactList[ j ].normal = ruVector3( pt.m_normalWorldOnB.x(), pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z());                
                nodeB->mContactList[ j ].position = ruVector3( pt.m_positionWorldOnB.x(), pt.m_positionWorldOnB.y(), pt.m_positionWorldOnB.z());                
                nodeB->mContactList[ j ].impulse = pt.m_appliedImpulse;				
				nodeB->mContactList[ j ].body = nodeA;
				if( obBIndex >= 0 ) {
					if( nodeB->mMeshList[obBIndex]->GetDiffuseTexture() ) {
						nodeB->mContactList[ j ].textureName = nodeB->mMeshList[obBIndex]->GetDiffuseTexture()->GetName();
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

		/*
        if ( pname == "octree" ) {
            if ( value == "1" ) {
                for( auto mesh : mMeshList ) {
                    if( !mesh->mOctree ) {
                        // mesh->octree = new Octree( mesh, 4096 );
                    }
                }
            }
        }
		*/
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

bool SceneNode::IsInsideNode( shared_ptr<ruSceneNode> n ) {
	shared_ptr<SceneNode> & node = std::dynamic_pointer_cast<SceneNode>( n );

    if( !node ) {
        return 0;
    }

    btVector3 pos( mGlobalTransform.getOrigin());

    ruVector3 point( pos.x(), pos.y(), pos.z() );

    int result = 0;

    btVector3 n2Pos = node->mGlobalTransform.getOrigin();
    for( auto mesh : node->mMeshList ) {
        AABB aabb = mesh->GetBoundingBox();

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

    for( auto & child : node->mChildList ) {
		result += child->IsInsideNode( shared_from_this() );
    }

    return result;
}

shared_ptr<ruSceneNode> SceneNode::GetChild( int i ) {
    return mChildList[i];
}

int SceneNode::GetCountChildren() {
    return mChildList.size();
}

shared_ptr<SceneNode> SceneNode::FindByName( const string & name ) {
	auto & nodes = SceneFactory::GetNodeList();
    for( auto & pWeak : nodes ) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if( node ) {
			if( node->mName == name ) {
				return node;
			}
        }
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
    for( auto & node : mChildList ) {
		if( node ) {
			node->SetDepthHack( depthHack );
		}
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

void SceneNode::SetLocalScale( ruVector3 scale ) {
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

shared_ptr<SceneNode>  FindChildInNode( shared_ptr<SceneNode> node, const string & name ) {
	if( node->mName == name ) {
		return node;
	}	
	for( auto & child : node->mChildList ) {
		shared_ptr<SceneNode> & lookup = FindChildInNode( child, name );
		if( lookup ) {
			return lookup;
		}		
	}
	return nullptr;
}

shared_ptr<ruSceneNode> SceneNode::FindChild( const string & name ) {
    return FindChildInNode( shared_from_this(), name );
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
        for( auto & child : mChildList ) {
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

shared_ptr<ruSceneNode> SceneNode::GetParent() {
	return mParent.lock();
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

shared_ptr<ruTexture> SceneNode::GetTexture( int n ) {
	if( n < 0 || n >= mMeshList.size() ) {
		return nullptr;
	} else {
		return mMeshList[n]->GetDiffuseTexture();
	}
}

int SceneNode::GetTextureCount() {
	return mMeshList.size();
}

float SceneNode::GetBlurAmount() {
	return mBlurAmount;
}

void SceneNode::SetBlurAmount( float blurAmount ) {
	mBlurAmount = blurAmount;
	for( auto child : mChildList ) {
		child->SetBlurAmount( blurAmount );
	}
}

void SceneNode::SetAlbedo( float albedo ) {
	mAlbedo = albedo;
}

bool SceneNode::IsInFrustum() {
	return mInFrustum;
}

void SceneNode::SetName( const string & name ) {
	mName = name;
}

int SceneNode::GetTotalAnimationFrameCount() {
	return mTotalFrameCount;
}

void SceneNode::AddMesh( const shared_ptr<Mesh> & mesh ) {
	mMeshList.push_back( mesh );
}

shared_ptr<ruSceneNode> ruSceneNode::Create( ) {
	return SceneFactory::CreateSceneNode();
}

shared_ptr<ruSceneNode> ruSceneNode::LoadFromFile( const string & file ) {
	return SceneNode::LoadScene( file );
}

shared_ptr<ruSceneNode> ruSceneNode::FindByName( const string & name ) {
	return SceneNode::FindByName( name );
}

shared_ptr<ruSceneNode> ruSceneNode::Duplicate( shared_ptr<ruSceneNode> source ) {
	return SceneFactory::CreateSceneNodeDuplicate( dynamic_pointer_cast<SceneNode>( source ));
}

int ruSceneNode::GetWorldObjectsCount() {
	return SceneFactory::GetNodeList().size();
}

shared_ptr<ruSceneNode> ruSceneNode::GetWorldObject( int i ) {
	return SceneFactory::GetNodeList()[i].lock();
}

ruSceneNode::~ruSceneNode( ) {

}

void ruObject::Free() {
	delete this;
}