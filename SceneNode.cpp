#include "Common.h"
#include "FastReader.h"
#include "ParticleEmitter.h"
#include "Light.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Octree.h"
#include "Texture.h"
#include "Vertex.h"
#include "Renderer.h"

vector< SceneNode* > g_nodes;

SceneNode * SceneNode::CastHandle( ruNodeHandle handle ) {
    return reinterpret_cast< SceneNode *>( handle.pointer );
}

ruNodeHandle SceneNode::HandleFromPointer( SceneNode * ptr ) {
    ruNodeHandle handle;
    handle.pointer = ptr;
    return handle;
}

void SceneNode::EraseUnusedNodes() {
    for( auto node : g_nodes )
        for( auto iter = node->mChildList.begin(); iter != node->mChildList.end();  )
            if( *iter == 0 ) {
                node->mChildList.erase( iter );
            } else {
                ++iter;
            }
}

bool SceneNode::IsVisible() {
    bool trulyVisible = mVisible;

    if( mParent ) {
        trulyVisible &= mParent->IsVisible();
    }

    return trulyVisible;
}

bool SceneNode::IsRenderable() {
    return IsVisible();
}

SceneNode::SceneNode( ) {
    char buf[ 64 ] = { 0 };
    sprintf_s( buf, "Unnamed%i", g_nodes.size());
    mName = buf;
    mInFrustum = false;
    mParent = 0;
    mBody = 0;
    mTotalFrameCount = 0;
    mSkinned = false;
    trimesh = 0;
    mVisible = true;
    mAnimationEnabled = false;
    mScene = 0;
    mLocalTransform = btTransform( btQuaternion( 0, 0, 0 ), btVector3( 0, 0, 0 ));
    mGlobalTransform = mLocalTransform;
    mContactCount = 0;
    mFrozen = false;
    mDepthHack = 0;
    particleEmitter = 0;
    mAlbedo = 0.0f;
    mCurrentAnimation = nullptr;
    g_nodes.push_back( this );
	
}

void SceneNode::SetConvexBody() {
    if( mMeshList.size() == 0 ) {
        return;
    }

    btConvexHullShape * convex = new btConvexHullShape();

    for( auto mesh : mMeshList ) {
        for( auto & vertex : mesh->mVertices ) {
            convex->addPoint ( btVector3( vertex.coords.x, vertex.coords.y, vertex.coords.z ));
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
    if( mBody ) {
        mBody->setAngularFactor( btVector3( fact.x, fact.y, fact.z ) );
    }
}

void SceneNode::SetTrimeshBody() {
    if( mMeshList.size() == 0 ) {
        return;
    }

    btVector3 inertia ( 0.0f, 0.0f, 0.0f );
    trimesh = new btTriangleMesh();

    for ( auto mesh : mMeshList ) {
        for( auto triangle : mesh->mTriangles ) {
            ruVector3 & a = mesh->mVertices[ triangle.mA ].coords;
            ruVector3 & b = mesh->mVertices[ triangle.mB ].coords;
            ruVector3 & c = mesh->mVertices[ triangle.mC ].coords;

            trimesh->addTriangle ( btVector3( a.x, a.y, a.z ), btVector3( b.x, b.y, b.z ), btVector3( c.x, c.y, c.z ), false );
        };
    }

    SetBody(new btRigidBody(0, (btMotionState*)(new btDefaultMotionState()), (btCollisionShape*) (new btBvhTriangleMeshShape(trimesh, true, true)), inertia));
	mBody->setLinearFactor( btVector3( 0,0,0 ));
	mBody->setAngularFactor( btVector3( 0,0,0 ));
}

void SceneNode::EraseChild( const SceneNode * child ) {
    auto childIterator = find( mChildList.begin(), mChildList.end(), child );

    if( childIterator != mChildList.end() ) {
        mChildList.erase( childIterator );
    }
}

void SceneNode::AttachTo( SceneNode * newParent ) {
    if( !newParent ) {
        return;
    }

    mParent = newParent;
    newParent->mChildList.push_back( this );
}

btTransform & SceneNode::CalculateGlobalTransform() {
    if( mBody ) {
        if( mParent ) {
            if( mFrozen ) { // only frozen bodies can be parented
                mGlobalTransform = mParent->CalculateGlobalTransform() * mLocalTransform;
                mBody->setWorldTransform( mGlobalTransform );
                mBody->setLinearVelocity( btVector3( 0, 0, 0 ));
                mBody->setAngularVelocity( btVector3( 0, 0, 0 ));
            } else {
                mGlobalTransform = mBody->getWorldTransform();
            }
        } else { // dont has parent
            mGlobalTransform = mBody->getWorldTransform();
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
        MessageBoxA( 0, Format( "Unable to load '%s' scene!", file ).c_str(), 0, MB_OK | MB_ICONERROR );
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
        node->mSkinned = isSkinned;
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
            Mesh * mesh = new Mesh( node );

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

                v.coords = reader.GetBareVector();
                v.normals = reader.GetBareVector();
                v.texCoords = reader.GetBareVector2();
                ruVector2 tc2 = reader.GetBareVector2(); // just read secondary texcoords, but don't add it to the mesh. need to compatibility
                v.tangents = reader.GetBareVector();

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

            mesh->mDiffuseTexture = Texture::Require( g_texturePath + diffuse );
			if( mesh->mOpacity > 0.95f )
				mesh->mNormalTexture = Texture::Require( g_texturePath + normal );

            node->mMeshList.push_back( mesh );

            if( node->mSkinned ) {
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

int SceneNode::IsAnimationEnabled() {
    int animCount = mAnimationEnabled ? 1 : 0;

    for( auto child : mChildList ) {
        animCount += child->IsAnimationEnabled();
    }

    return animCount;
}


void SceneNode::PerformAnimation() {
    if ( mSkinned ) {
        int vertexNumber = 0;
        mParent = nullptr;
        for( auto mesh : mMeshList ) {
            mesh->mSkinVertices = mesh->mVertices;
            for( auto & vertex : mesh->mVertices ) {
                Mesh::Weight & weight = mesh->mWeightTable[ vertexNumber ];

                btVector3 initialPosition = btVector3( vertex.coords.x, vertex.coords.y, vertex.coords.z );
                btVector3 initialNormal = btVector3( vertex.normals.x, vertex.normals.y, vertex.normals.z );
                btVector3 initialTangent = btVector3( vertex.tangents.x, vertex.tangents.y, vertex.tangents.z );
                btVector3 newPosition = btVector3( 0, 0, 0 );
                btVector3 newNormal = btVector3( 0, 0, 0 );
                btVector3 newTangent = btVector3( 0, 0, 0 );

                for( int j = 0; j < weight.mBoneCount; j++ ) {
                    Mesh::Bone & bone = weight.mBone[ j ];
                    SceneNode * boneNode = mScene->mChildList[ bone.mID ];

                    btTransform transform = ( boneNode->mGlobalTransform * boneNode->mInvBoneBindTransform ) * mGlobalTransform;
                    newPosition += transform * initialPosition * bone.mWeight;
                    newNormal += transform.getBasis() * initialNormal * bone.mWeight;
                    newTangent += transform.getBasis() * initialTangent * bone.mWeight;
                }

                vertex.coords = ruVector3( newPosition.m_floats );
                vertex.normals = ruVector3( newNormal.m_floats );
                vertex.tangents = ruVector3( newTangent.m_floats );

                vertexNumber++;
            }
            mesh->UpdateBuffers();
            mesh->mVertices = mesh->mSkinVertices;
        }
    } else {
        if( mCurrentAnimation ) {
            if( mAnimationEnabled ) {
                if ( mKeyframeList.size() ) {
                    btTransform * currentFrameTransform = mKeyframeList[ mCurrentAnimation->currentFrame ];
                    btTransform * nextFrameTransform = mKeyframeList[ mCurrentAnimation->nextFrame ];
                    mLocalTransform.setRotation( currentFrameTransform->getRotation().slerp( nextFrameTransform->getRotation(), mCurrentAnimation->interpolator ));
                    mLocalTransform.setOrigin( currentFrameTransform->getOrigin().lerp( nextFrameTransform->getOrigin(), mCurrentAnimation->interpolator ));
                }
            }
        }
    }
}

void SceneNode::Freeze() {
    mFrozen = true;
    if( !mBody ) {
        return;
    }
    mBody->setAngularFactor( 0 );
    mBody->setLinearFactor( btVector3( 0, 0, 0 ));
    mBody->setAngularVelocity( btVector3( 0, 0, 0 ));
    mBody->setLinearVelocity( btVector3( 0, 0, 0 ));
    mBody->setGravity( btVector3( 0, 0, 0 ));
}

void SceneNode::Unfreeze() {
    mFrozen = false;
    if( !mBody ) {
        return;
    }
	mBody->activate(true);
    mBody->setAngularFactor( 1 );
    mBody->setLinearFactor( btVector3( 1, 1, 1 ));
    mBody->setGravity( g_dynamicsWorld->getGravity() );
}

void SceneNode::SetAnimationEnabled( bool state, bool dontAffectChilds ) {
    if( mCurrentAnimation ) {
        mAnimationEnabled = state;

        if( !dontAffectChilds ) {
            for( auto child : mChildList ) {
                child->SetAnimationEnabled( state );
            }
        }
    }
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
    int numManifolds = g_dynamicsWorld->getDispatcher()->getNumManifolds();

    for( auto node : g_nodes )
        if( node->mBody ) {
            node->mContactCount = 0;
        }

    for (int i=0; i < numManifolds; i++) {
        btPersistentManifold* contactManifold = g_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
        const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());

        SceneNode * nodeA = static_cast<SceneNode*>( obA->getUserPointer() );
        SceneNode * nodeB = static_cast<SceneNode*>( obB->getUserPointer() );

        if( !nodeA || !nodeB ) {
            continue;
        }

        int numContacts = contactManifold->getNumContacts();

        /*
        nodeA->numContacts += numContacts;
        nodeB->numContacts += numContacts;*/

        if( numContacts > BODY_MAX_CONTACTS ) {
            numContacts = BODY_MAX_CONTACTS;
        }

        for (int j = 0 ; j < numContacts; j++ ) {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);

            if (pt.getDistance() < 0.f ) {
                nodeA->mContactCount++;
                nodeB->mContactCount++;

                nodeA->mContactList[ j ].normal = ruVector3( pt.m_normalWorldOnB.x(), pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z());
                nodeB->mContactList[ j ].normal = ruVector3( pt.m_normalWorldOnB.x(), pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z());
                nodeA->mContactList[ j ].position = ruVector3( pt.m_positionWorldOnA.x(), pt.m_positionWorldOnA.y(), pt.m_positionWorldOnA.z());
                nodeB->mContactList[ j ].position = ruVector3( pt.m_positionWorldOnB.x(), pt.m_positionWorldOnB.y(), pt.m_positionWorldOnB.z());

                nodeA->mContactList[ j ].impulse = pt.m_appliedImpulse;
                nodeB->mContactList[ j ].impulse = pt.m_appliedImpulse;

				nodeA->mContactList[ j ].body.pointer = nodeB;
				nodeB->mContactList[ j ].body.pointer = nodeA;

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
    if( mBody ) {
        mBody->setLinearFactor( btVector3( lin.x, lin.y, lin.z ) );
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

        if ( pname == "mass" )
            if( mBody ) {
                SetMass( atof( value.c_str()) );
            }

        if ( pname == "friction" )
            if( mBody ) {
                mBody->setFriction ( atof( value.c_str()) );
            }

        if ( pname == "hitSound" ) {
            mHitSound = ruLoadSound3D( value.c_str());
            AttachSound( mHitSound );
        };

        if ( pname == "idleSound" ) {
            mIdleSound = ruLoadSound3D( value.c_str());
            AttachSound( mIdleSound );
        };
    };
}


void SceneNode::AttachSound( ruSoundHandle sound ) {
    mSoundList.push_back( sound );
}

bool SceneNode::IsNodeInside( SceneNode * node ) {
    if( !node ) {
        return 0;
    }

    btVector3 pos = mGlobalTransform.getOrigin();

    ruVector3 point = ruVector3( pos.x(), pos.y(), pos.z() );

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
    for( auto node : g_nodes )
        if( node->mName == name ) {
            return node;
        }
    return nullptr;
}

SceneNode::~SceneNode() {
    for( auto mesh : mMeshList ) {
        if( mesh ) {
            delete mesh;
        }
    }

    for( auto child : mChildList ) {
        if( child ) {
            delete child;
        }
    }

    for( auto theNode : g_nodes ) {
        for( size_t i = 0; i < theNode->mChildList.size(); i++ ) {
            if( theNode->mChildList[i] == this ) {
                theNode->mChildList[i] = nullptr;
            }
        }
    }

    for( auto keyframe : mKeyframeList ) {
        delete keyframe;
    }

    if( trimesh ) {
        delete trimesh;
    }

    if( mBody ) {
        if( mBody->getCollisionShape() ) {
            delete mBody->getCollisionShape();
        }

        if( mBody->getMotionState() ) {
            delete mBody->getMotionState();
        }

        g_dynamicsWorld->removeRigidBody( mBody );

        delete mBody;
    }

    if( particleEmitter ) {
        delete particleEmitter;
    }

    g_nodes.erase( find( g_nodes.begin(), g_nodes.end(), this ));
}

void SceneNode::SetFriction( float friction ) {
    if( mBody ) {
        mBody->setFriction( friction );
    }
}

void SceneNode::SetDepthHack( float depthHack ) {
    mDepthHack = depthHack;
    for( size_t i = 0; i < mChildList.size(); i++ ) {
        mChildList[ i ]->SetDepthHack( depthHack );
    }
}

void SceneNode::SetAnisotropicFriction( ruVector3 aniso ) {
    if( mBody ) {
        mBody->setAnisotropicFriction( btVector3( aniso.x, aniso.y, aniso.z ));
    }
}

void SceneNode::Move( ruVector3 speed ) {
    if( mBody ) {
		mBody->activate( true );
        mBody->setLinearVelocity(  btVector3( speed.x, speed.y, speed.z ) );
    };
	mLocalTransform.setOrigin( mLocalTransform.getOrigin() + btVector3( speed.x, speed.y, speed.z ) );
}

void SceneNode::SetVelocity( ruVector3 velocity ) {
    if( mBody ) {
		mBody->activate( true );
        mBody->setLinearVelocity( btVector3( velocity.x, velocity.y, velocity.z ) );
    }
}

void SceneNode::SetPosition( ruVector3 position ) {
	mLocalTransform.setOrigin( btVector3( position.x, position.y, position.z ) );
    if( mBody ) {
		mBody->activate(true);
        mBody->getWorldTransform().setOrigin( btVector3( position.x, position.y, position.z ) );
    };
	CalculateGlobalTransform();
}

float SceneNode::GetMass() {
    if( mBody ) {
        return 1.0f / mBody->getInvMass();
    }

    return 0.0f;
}

bool SceneNode::IsFrozen() {
    return mFrozen;
}

void SceneNode::SetRotation( ruQuaternion rotation ) {
    if( mBody ) {
		mBody->activate( true );
        mBody->getWorldTransform().getBasis().setRotation( btQuaternion( rotation.x, rotation.y, rotation.z, rotation.w ) );
    }
	mLocalTransform.setRotation( btQuaternion( rotation.x, rotation.y, rotation.z, rotation.w ));
}

void SceneNode::SetBodyLocalScaling( ruVector3 scale ) {
	if( mBody ) {
		mBody->getCollisionShape()->setLocalScaling( btVector3( scale.x, scale.y, scale.z ));
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

    if( mBody && !mFrozen ) {
        transform = mBody->getWorldTransform();
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
    if( mBody ) {
        mBody->setAngularVelocity( btVector3( velocity.x, velocity.y, velocity.z ));
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
    if( mBody ) {
        transform = mBody->getWorldTransform();
    }
    return ruQuaternion( transform.getRotation().x(), transform.getRotation().y(), transform.getRotation().z(), transform.getRotation().w() );
}

void SceneNode::SetDamping( float linearDamping, float angularDamping ) {
    if( mBody ) {
        mBody->setDamping( linearDamping, angularDamping );
    }
}

void SceneNode::SetGravity( const ruVector3 & gravity ) {
    btVector3 g( gravity.x, gravity.y, gravity.z );

    if(mBody) {
        mBody->setGravity( g );
    }
}

void SceneNode::SetMass( float mass ) {
    if( mBody ) {
        btVector3 inertia;
        mBody->getCollisionShape()->calculateLocalInertia( mass, inertia );
        mBody->setMassProps( mass, inertia );
    }
}

void SceneNode::SetBody( btRigidBody * theBody ) {
    mBody = theBody;
    mBody->setWorldTransform ( mGlobalTransform );
    mBody->setFriction(1.0f);
    mBody->setUserPointer( this );
    mBody->setRestitution( 0.0f );
	mBody->setDeactivationTime( 0.1f );
	mBody->setCcdMotionThreshold( 0.75f );
	mBody->setCcdSweptSphereRadius( 0.2f );
	mBody->setSleepingThresholds( 1.0f, 1.0f );
	mBody->getCollisionShape()->setMargin(0.02);
    g_dynamicsWorld->addRigidBody ( mBody );
}

void SceneNode::SetAnimation( ruAnimation * newAnim, bool dontAffectChilds ) {
    mCurrentAnimation = newAnim;
    if( !dontAffectChilds ) {
        for( auto child : mChildList ) {
            child->SetAnimation( newAnim, false );
        }
    }
}

BodyType SceneNode::GetBodyType()
{
	BodyType bodyType = BodyType::None;
	if( mBody ) {
		btCollisionShape * shape = mBody->getCollisionShape();
		if( dynamic_cast<btSphereShape*>( shape )) {
			bodyType = BodyType::Sphere;
		}
		if( dynamic_cast<btBvhTriangleMeshShape*>( shape )) {
			bodyType = BodyType::Trimesh;
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

ruVector3 SceneNode::GetTotalForce()
{
	return ruVector3(mBody->getTotalForce().x(), mBody->getTotalForce().y(), mBody->getTotalForce().z());
}

ruAnimation * SceneNode::GetCurrentAnimation() {
	return mCurrentAnimation;
}

ruVector3 SceneNode::GetLinearVelocity() {
	ruVector3 vel;
	if( mBody ) {
		vel.x = mBody->getLinearVelocity().x();
		vel.y = mBody->getLinearVelocity().y();
		vel.z = mBody->getLinearVelocity().z();
	}
	return vel;
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

bool ruNodeHandle::operator == ( const ruNodeHandle & node ) {
    return pointer == node.pointer;
}

ruVector3 ruGetNodeLinearVelocity( ruNodeHandle node ) {
	return SceneNode::CastHandle( node )->GetLinearVelocity();
}

void ruCreateOctree( ruNodeHandle node, int splitCriteria ) {
    SceneNode * n = SceneNode::CastHandle( node );

    for( int i = 0; i < n->mMeshList.size(); i++ ) {
        Mesh * mesh = n->mMeshList[ i ];

        if( mesh->mOctree ) {
            delete mesh->mOctree;
        }

        mesh->mOctree = new Octree( mesh, splitCriteria );
    }
}

void ruSetNodeGravity( ruNodeHandle node, ruVector3 gravity ) {
    SceneNode::CastHandle( node )->SetGravity( gravity );
}

void ruSetNodeDamping( ruNodeHandle node, float linearDamping, float angularDamping ) {
    SceneNode::CastHandle( node )->SetDamping( linearDamping, angularDamping );
}

void ruSetNodeMass( ruNodeHandle node, float mass ) {
    SceneNode::CastHandle( node )->SetMass( mass );
}

void ruDeleteOctree( ruNodeHandle node ) {
    SceneNode * n = SceneNode::CastHandle( node );

    for( int i = 0; i < n->mMeshList.size(); i++ ) {
        Mesh * mesh = n->mMeshList[ i ];

        if( mesh->mOctree ) {
            delete mesh->mOctree;
        }
    }
}

void ruDetachNode( ruNodeHandle node ) {
    SceneNode * n = SceneNode::CastHandle( node );

    if( n->mParent ) {
        n->mParent->EraseChild( n );
    }

    n->mParent = 0;
}

bool ruIsNodeVisible( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->mVisible;
}

bool ruIsNodeInFrustum( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->mInFrustum;
}

void ruAttachSound( ruSoundHandle sound, ruNodeHandle node ) {
    SceneNode::CastHandle( node )->AttachSound( sound );
}

ruNodeHandle ruCreateSceneNode( ) {
    return SceneNode::HandleFromPointer( new SceneNode );
}

ruNodeHandle ruFindInObjectByName( ruNodeHandle node, const string & name ) {
    return SceneNode::HandleFromPointer( SceneNode::FindInObjectByName( SceneNode::CastHandle( node ), name ));
}

ruVector3 ruGetNodeEulerAngles( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetEulerAngles();
}

void ruSetConvexBody( ruNodeHandle node ) {
    SceneNode::CastHandle( node )->SetConvexBody();
}

void ruSetCapsuleBody( ruNodeHandle node, float height, float radius ) {
    SceneNode::CastHandle( node )->SetCapsuleBody( height, radius );
}

void ruSetAngularFactor( ruNodeHandle node, ruVector3 fact ) {
    SceneNode::CastHandle( node )->SetAngularFactor( fact );
}

void ruSetTrimeshBody( ruNodeHandle node ) {
    SceneNode::CastHandle( node )->SetTrimeshBody();
}

void ruAttachNode( ruNodeHandle node1, ruNodeHandle node2 ) {
    SceneNode::CastHandle( node1 )->AttachTo( SceneNode::CastHandle( node2 ) );
}

ruVector3 ruGetNodeAABBMin( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetAABBMin();
}

ruVector3 ruGetNodeBodyTotalForce( ruNodeHandle node ) {
	return SceneNode::CastHandle( node )->GetTotalForce();
}

ruVector3 ruGetNodeAABBMax( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetAABBMax();
}

ruNodeHandle ruLoadScene( const string & file ) {
    return SceneNode::HandleFromPointer( SceneNode::LoadScene( file ));
}

void ruFreeze( ruNodeHandle node ) {
    SceneNode::CastHandle( node )->Freeze();
}

void ruUnfreeze( ruNodeHandle node ) {
    SceneNode::CastHandle( node )->Unfreeze();
}

void ruHideNode( ruNodeHandle node ) {
    SceneNode::CastHandle( node )->Hide();
}

void ruShowNode( ruNodeHandle node ) {
    SceneNode::CastHandle( node )->Show();
}

string ruGetProperty( ruNodeHandle node, string propName ) {
    return SceneNode::CastHandle( node )->GetProperty( propName );
}

void ruSetNodeLinearFactor( ruNodeHandle node, ruVector3 lin ) {
    SceneNode::CastHandle( node )->SetLinearFactor( lin );
}

ruVector3 ruGetNodePosition( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetPosition();
}

int ruGetContactCount( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetContactCount();
}

ruContact ruGetContact( ruNodeHandle node, int num ) {
    return SceneNode::CastHandle( node )->GetContact( num );
}

int ruIsNodeInsideNode( ruNodeHandle node1, ruNodeHandle node2 ) {
    return SceneNode::CastHandle( node1 )->IsNodeInside( SceneNode::CastHandle( node2 ));
}

ruNodeHandle ruGetNodeChild( ruNodeHandle node, int i ) {
    return SceneNode::HandleFromPointer( SceneNode::CastHandle( node )->GetChild( i ));
}

int ruGetNodeCountChildren( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetCountChildren();
}

ruNodeHandle ruFindByName( const string & name ) {
    return SceneNode::HandleFromPointer( SceneNode::FindByName( name ));
}

void ruFreeSceneNode( ruNodeHandle node ) {
    delete SceneNode::CastHandle( node );
}

void ruSetNodeFriction( ruNodeHandle node, float friction ) {
    SceneNode::CastHandle( node )->SetFriction( friction );
}

void ruSetNodeAlbedo( ruNodeHandle node, float albedo ) {
    SceneNode::CastHandle( node )->mAlbedo = albedo;
}

void ruSetNodeDepthHack( ruNodeHandle node, float depthHack ) {
    SceneNode::CastHandle( node )->SetDepthHack( depthHack );
}

void ruSetNodeAnisotropicFriction( ruNodeHandle node, ruVector3 aniso ) {
    SceneNode::CastHandle( node )->SetAnisotropicFriction( aniso );
}

bool ruIsNodeHasBody( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->mBody != nullptr;
}

void ruMoveNode( ruNodeHandle node, ruVector3 speed ) {
    SceneNode::CastHandle( node )->Move( speed );
}

void ruSetNodeVelocity( ruNodeHandle node, ruVector3 velocity ) {
    SceneNode::CastHandle( node )->SetVelocity( velocity );
}

void ruSetNodeAngularVelocity( ruNodeHandle node, ruVector3 velocity ) {
    SceneNode::CastHandle( node )->SetAngularVelocity( velocity );
}

void ruSetNodePosition( ruNodeHandle node, ruVector3 position ) {
    SceneNode::CastHandle( node )->SetPosition( position );
}

float ruGetNodeMass( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetMass();
}

int ruIsNodeFrozen( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->IsFrozen();
}

void ruSetNodeRotation( ruNodeHandle node, ruQuaternion rotation ) {
    SceneNode::CastHandle( node )->SetRotation( rotation );
}

ruVector3 ruGetNodeLookVector( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetLookVector();
}

const string & ruGetNodeName( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetName();
}

ruVector3 ruGetNodeRightVector( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetRightVector();
}

ruVector3 ruGetNodeUpVector( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetUpVector();
}

ruVector3 ruGetNodeLocalPosition( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetLocalPosition();
}

ruQuaternion ruGetNodeLocalRotation( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetLocalRotation();
}

RUAPI ruVector3 ruGetNodeAbsoluteLookVector( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetAbsoluteLookVector();
}

void ruSetNodeLocalPosition( ruNodeHandle node, ruVector3 pos ) {
    SceneNode * s = SceneNode::CastHandle( node );
    s->mLocalTransform.setOrigin( btVector3( pos.x, pos.y, pos.z ));
    if( s->mBody ) {
        s->mBody->getWorldTransform().setOrigin( btVector3( pos.x, pos.y, pos.z ) );
    }
    SceneNode::CastHandle( node )->CalculateGlobalTransform( );
}

BodyType ruGetNodeBodyType( ruNodeHandle node ) {
	return SceneNode::CastHandle( node )->GetBodyType();	
}

void ruSetNodeLocalRotation( ruNodeHandle node, ruQuaternion rot ) {
    SceneNode * s = SceneNode::CastHandle( node );
    s->mLocalTransform.setRotation( btQuaternion( rot.x, rot.y, rot.z, rot.w ));
    if( s->mBody ) {
        s->mBody->getWorldTransform().setRotation( btQuaternion( rot.x, rot.y, rot.z, rot.w ) );
    }
    SceneNode::CastHandle( node )->CalculateGlobalTransform( );
}

void ruSetNodeName( ruNodeHandle node, const string & name ) {
    SceneNode::CastHandle( node )->mName = name;
}

int ruGetWorldObjectsCount() {
    return g_nodes.size();
}

RUAPI ruNodeHandle ruGetWorldObject( int i ) {
    ruNodeHandle handle;
    handle.pointer = g_nodes[ i ];
    return handle;
}

// animation
int ruIsAnimationEnabled( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->IsAnimationEnabled();
}

void ruSetAnimationEnabled( ruNodeHandle node, bool state, bool dontAffectChilds ) {
    SceneNode::CastHandle( node )->SetAnimationEnabled( state, dontAffectChilds );
}

void ruSetAnimation( ruNodeHandle node, ruAnimation * newAnim, bool dontAffectChilds ) {
    SceneNode::CastHandle( node )->SetAnimation( newAnim, dontAffectChilds );
}

int ruGetTotalAnimationFrameCount( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->mTotalFrameCount;
}

ruAnimation * ruGetCurrentAnimation( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->mCurrentAnimation;
}

void ruSetNodeBodyLocalScale( ruNodeHandle node, ruVector3 scale ) {
	return SceneNode::CastHandle( node )->SetBodyLocalScaling( scale );
}