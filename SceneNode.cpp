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
        for( auto iter = node->childs.begin(); iter != node->childs.end();  )
            if( *iter == 0 ) {
                node->childs.erase( iter );
            } else {
                ++iter;
            }
}

bool SceneNode::IsVisible() {
    bool trulyVisible = visible;

    if( parent ) {
        trulyVisible &= parent->IsVisible();
    }

    return trulyVisible;
}

bool SceneNode::IsRenderable() {
    return IsVisible();
}

SceneNode::SceneNode( ) {
    char buf[ 64 ] = { 0 };
    sprintf_s( buf, "Unnamed%i", g_nodes.size());
    name = buf;
    inFrustum = false;
    parent = 0;
    body = 0;
    totalFrames = 0;
    skinned = false;
    trimesh = 0;
    visible = true;
    animationEnabled = false;
    scene = 0;
    localTransform = btTransform( btQuaternion( 0, 0, 0 ), btVector3( 0, 0, 0 ));
    globalTransform = localTransform;
    numContacts = 0;
    frozen = false;
    fDepthHack = 0;
    particleEmitter = 0;
    albedo = 0.0f;
    currentAnimation = nullptr;
    g_nodes.push_back( this );
}

void SceneNode::SetConvexBody() {
    if( meshes.size() == 0 ) {
        return;
    }

    btConvexHullShape * convex = new btConvexHullShape();

    for( auto mesh : meshes ) {
        for( auto & vertex : mesh->vertices ) {
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
    if( body ) {
        body->setAngularFactor( btVector3( fact.x, fact.y, fact.z ) );
    }
}

void SceneNode::SetTrimeshBody() {
    if( meshes.size() == 0 ) {
        return;
    }

    btVector3 inertia ( 0.0f, 0.0f, 0.0f );
    trimesh = new btTriangleMesh ( true, false );

    for ( auto mesh : meshes ) {
        for( auto triangle : mesh->triangles ) {
            ruVector3 & a = mesh->vertices[ triangle.a ].coords;
            ruVector3 & b = mesh->vertices[ triangle.b ].coords;
            ruVector3 & c = mesh->vertices[ triangle.c ].coords;

            trimesh->addTriangle ( btVector3( a.x, a.y, a.z ), btVector3( b.x, b.y, b.z ), btVector3( c.x, c.y, c.z ) );
        };
    }

    SetBody( new btRigidBody ( 0, ( btMotionState * ) ( new btDefaultMotionState() ), ( btCollisionShape * ) ( new btBvhTriangleMeshShape ( trimesh, true, true ) ), inertia ));
}

void SceneNode::EraseChild( const SceneNode * child ) {
    auto childIterator = find( childs.begin(), childs.end(), child );

    if( childIterator != childs.end() ) {
        childs.erase( childIterator );
    }
}

void SceneNode::AttachTo( SceneNode * newParent ) {
    if( !newParent ) {
        return;
    }

    parent = newParent;
    newParent->childs.push_back( this );
}

btTransform & SceneNode::CalculateGlobalTransform() {
    if( body )
        if( body->wantsSleeping() ) {
            body->activate( true );
        }

    if( body ) {
        if( parent ) {
            if( frozen ) { // only frozen bodies can be parented
                globalTransform = parent->CalculateGlobalTransform() * localTransform;

                body->setWorldTransform( globalTransform );
                body->setLinearVelocity( btVector3( 0, 0, 0 ));
                body->setAngularVelocity( btVector3( 0, 0, 0 ));
            } else {
                globalTransform = body->getWorldTransform();
            }
        } else { // dont has parent
            globalTransform = body->getWorldTransform();
        }
    } else { // dont has body
        if( parent ) {
            globalTransform = parent->CalculateGlobalTransform() * localTransform;
        } else {
            globalTransform = localTransform;
        }
    }

    return globalTransform;
}

ruVector3 SceneNode::GetAABBMin() {
    ruVector3 min = ruVector3( FLT_MAX, FLT_MAX, FLT_MAX );

    for( auto mesh : meshes ) {
        AABB aabb = mesh->aabb;
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

    for( auto mesh : meshes ) {
        AABB aabb = mesh->aabb;

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
    for( auto child : parent->childs )
        if( child->name == childName ) {
            return child;
        }
    return nullptr;
}

SceneNode * SceneNode::LoadScene( const char * file ) {
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

    scene->totalFrames = framesCount;

    for ( int meshObjectNum = 0; meshObjectNum < numMeshes; meshObjectNum++ ) {
        SceneNode * node = new SceneNode;

        node->localTransform.setOrigin( reader.GetVector() );
        node->localTransform.setRotation( reader.GetQuaternion() );
        node->globalTransform = node->localTransform;

        int hasAnimation = reader.GetInteger();
        int isSkinned = reader.GetInteger();
        int meshCount = reader.GetInteger();
        int keyframeCount = reader.GetInteger();
        node->skinned = isSkinned;
        ParseString( reader.GetString(), node->properties );
        node->name = reader.GetString();
        for( int i = 0; i < keyframeCount; i++ ) {
            btTransform * keyframe = new btTransform;
            keyframe->setOrigin( reader.GetVector());
            keyframe->setRotation( reader.GetQuaternion());
            node->keyframes.push_back( keyframe );
        }

        if( keyframeCount ) {
            node->localTransform = *node->keyframes[ 0 ];
        }

        node->totalFrames = framesCount - 1; // - 1 because numeration started from zero

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
            mesh->opacity = reader.GetFloat() / 100.0f;

            mesh->vertices.reserve( vertexCount );
            for( int vertexNum = 0; vertexNum < vertexCount; vertexNum++ ) {
                Vertex v;

                v.coords = reader.GetBareVector();
                v.normals = reader.GetBareVector();
                v.texCoords = reader.GetBareVector2();
                ruVector2 tc2 = reader.GetBareVector2(); // just read secondary texcoords, but don't add it to the mesh. need to compatibility
                v.tangents = reader.GetBareVector();

                mesh->vertices.push_back( v );
            }

            mesh->aabb = AABB( mesh->vertices );

            mesh->triangles.reserve( indexCount );

            for( int indexNum = 0; indexNum < indexCount; indexNum += 3 ) {
                unsigned short a = reader.GetShort();
                unsigned short b = reader.GetShort();
                unsigned short c = reader.GetShort();

                mesh->triangles.push_back( Mesh::Triangle( a, b, c ));
            }

            mesh->diffuseTexture = Texture::Require( g_texturePath + diffuse );
            mesh->normalMapTexture = Texture::Require( g_texturePath + normal );

            node->meshes.push_back( mesh );

            if( node->skinned ) {
                for( int k = 0; k < vertexCount; k++ ) {
                    Mesh::Weight w;

                    w.boneCount = reader.GetInteger();

                    for( int j = 0; j < w.boneCount; j++ ) {
                        w.bones[ j ].id = reader.GetInteger();
                        w.bones[ j ].weight = reader.GetFloat();
                    }

                    mesh->weightTable.push_back( w );
                }
            }

            if( vertexCount != 0 ) {
                mesh->UpdateBuffers();
                Mesh::Register( mesh );
            }
        }

        node->parent = scene;
        scene->childs.push_back( node );
        node->scene = scene;
        node->ApplyProperties();
    }

    for( int lightObjectNum = 0; lightObjectNum < numLights; lightObjectNum++ ) {
        string name = reader.GetString();
        int type = reader.GetInteger();
        Light * light = new Light( type );
        light->name = name;
        light->SetColor( reader.GetBareVector());
        light->SetRadius( reader.GetFloat());
        light->brightness = reader.GetFloat();
        light->localTransform.setOrigin( reader.GetVector());
        light->scene = scene;
        light->parent = scene;
        scene->childs.push_back( light );
        if( type == LT_SPOT ) {
            float in = reader.GetFloat();
            float out = reader.GetFloat();
            light->SetConeAngles( in, out );
            light->localTransform.setRotation( reader.GetQuaternion());
        }
    }

    for( auto child : scene->childs ) {
        string objectName = reader.GetString();
        string parentName = reader.GetString();

        SceneNode * object = Find( scene, objectName );
        SceneNode * parent = Find( scene, parentName );

        if( parent ) {
            parent->childs.push_back( object );
            object->parent = parent;
        }
    }

    for( auto node : scene->childs ) {
        node->invBoneBindTransform = node->CalculateGlobalTransform().inverse();
    }

    return scene;
}

int SceneNode::IsAnimationEnabled() {
    int animCount = animationEnabled ? 1 : 0;

    for( auto child : childs ) {
        animCount += child->IsAnimationEnabled();
    }

    return animCount;
}


void SceneNode::PerformAnimation() {
    if ( skinned ) {
        int vertexNumber = 0;
        parent = nullptr;
        for( auto mesh : meshes ) {
            mesh->skinningBuffer = mesh->vertices;
            for( auto & vertex : mesh->vertices ) {
                Mesh::Weight & weight = mesh->weightTable[ vertexNumber ];

                btVector3 initialPosition = btVector3( vertex.coords.x, vertex.coords.y, vertex.coords.z );
                btVector3 initialNormal = btVector3( vertex.normals.x, vertex.normals.y, vertex.normals.z );
                btVector3 initialTangent = btVector3( vertex.tangents.x, vertex.tangents.y, vertex.tangents.z );
                btVector3 newPosition = btVector3( 0, 0, 0 );
                btVector3 newNormal = btVector3( 0, 0, 0 );
                btVector3 newTangent = btVector3( 0, 0, 0 );

                for( int j = 0; j < weight.boneCount; j++ ) {
                    Mesh::Bone & bone = weight.bones[ j ];
                    SceneNode * boneNode = scene->childs[ bone.id ];

                    btTransform transform = ( boneNode->globalTransform * boneNode->invBoneBindTransform ) * globalTransform;
                    newPosition += transform * initialPosition * bone.weight;
                    newNormal += transform.getBasis() * initialNormal * bone.weight;
                    newTangent += transform.getBasis() * initialTangent * bone.weight;
                }

                vertex.coords = ruVector3( newPosition.m_floats );
                vertex.normals = ruVector3( newNormal.m_floats );
                vertex.tangents = ruVector3( newTangent.m_floats );

                vertexNumber++;
            }
            mesh->UpdateBuffers();
            mesh->vertices = mesh->skinningBuffer;
        }
    } else {
        if( currentAnimation ) {
            if( animationEnabled ) {
                if ( keyframes.size() ) {
                    btTransform * currentFrameTransform = keyframes[ currentAnimation->currentFrame ];
                    btTransform * nextFrameTransform = keyframes[ currentAnimation->nextFrame ];
                    localTransform.setRotation( currentFrameTransform->getRotation().slerp( nextFrameTransform->getRotation(), currentAnimation->interpolator ));
                    localTransform.setOrigin( currentFrameTransform->getOrigin().lerp( nextFrameTransform->getOrigin(), currentAnimation->interpolator ));
                }
            }
        }
    }
}

void SceneNode::Freeze() {
    frozen = true;
    if( !body ) {
        return;
    }
    body->setAngularFactor( 0 );
    body->setLinearFactor( btVector3( 0, 0, 0 ));
    body->setAngularVelocity( btVector3( 0, 0, 0 ));
    body->setLinearVelocity( btVector3( 0, 0, 0 ));
    body->setGravity( btVector3( 0, 0, 0 ));
}

void SceneNode::Unfreeze() {
    frozen = false;
    if( !body ) {
        return;
    }
    body->setAngularFactor( 1 );
    body->setLinearFactor( btVector3( 1, 1, 1 ));
    body->setGravity( g_dynamicsWorld->getGravity() );
}

void SceneNode::SetAnimationEnabled( bool state, bool dontAffectChilds ) {
    if( currentAnimation ) {
        animationEnabled = state;

        if( !dontAffectChilds ) {
            for( auto child : childs ) {
                child->SetAnimationEnabled( state );
            }
        }
    }
}

void SceneNode::Hide() {
    visible = false;
}

void SceneNode::Show() {
    visible = true;
}

std::string SceneNode::GetProperty( string propName ) {
    auto propIter = properties.find( propName );

    if( propIter != properties.end()) {
        return propIter->second;
    }

    return string( "(Nothing)" );
}

void SceneNode::UpdateSounds() {
    btVector3 pos = globalTransform.getOrigin();

    for( auto sound : sounds ) {
        pfSetSoundPosition( sound.pfHandle, pos.x(), pos.y(), pos.z() );
    }

    if( idleSound.pfHandle ) {
        pfPlaySound( idleSound.pfHandle, true );
    }
}

void SceneNode::UpdateContacts() {
    int numManifolds = g_dynamicsWorld->getDispatcher()->getNumManifolds();

    for( auto node : g_nodes )
        if( node->body ) {
            node->numContacts = 0;
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
                nodeA->numContacts++;
                nodeB->numContacts++;

                nodeA->contacts[ j ].normal = ruVector3( pt.m_normalWorldOnB.x(), pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z());
                nodeB->contacts[ j ].normal = ruVector3( pt.m_normalWorldOnB.x(), pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z());
                nodeA->contacts[ j ].position = ruVector3( pt.m_positionWorldOnA.x(), pt.m_positionWorldOnA.y(), pt.m_positionWorldOnA.z());
                nodeB->contacts[ j ].position = ruVector3( pt.m_positionWorldOnB.x(), pt.m_positionWorldOnB.y(), pt.m_positionWorldOnB.z());

                nodeA->contacts[ j ].impulse = pt.m_appliedImpulse;
                nodeB->contacts[ j ].impulse = pt.m_appliedImpulse;

                if( pt.m_appliedImpulse > 10.0f ) {
                    if( !nodeA->frozen )
                        if( nodeA->hitSound.pfHandle ) {
                            pfPlaySound( nodeA->hitSound.pfHandle );
                        }
                    if( !nodeB->frozen )
                        if( nodeB->hitSound.pfHandle ) {
                            pfPlaySound( nodeB->hitSound.pfHandle );
                        }

                    float vol = pt.m_appliedImpulse / 20.0f;

                    if( vol > 1.0f ) {
                        vol = 1.0f;
                    }

                    if( nodeA->hitSound.pfHandle ) {
                        pfSetSoundVolume( nodeA->hitSound.pfHandle, vol );
                    }
                    if( nodeB->hitSound.pfHandle ) {
                        pfSetSoundVolume( nodeB->hitSound.pfHandle, vol );
                    }

                    float pc = pt.m_appliedImpulse / 30.0f;
                    if( pc > 0.7f ) {
                        pc = 0.7f;
                    }

                    float pitch = 0.6f + pc;

                    if( nodeA->hitSound.pfHandle ) {
                        pfSetSoundPitch( nodeA->hitSound.pfHandle, pitch );
                    }
                    if( nodeB->hitSound.pfHandle ) {
                        pfSetSoundPitch( nodeB->hitSound.pfHandle, pitch );
                    }
                }
            }
        }
    }
}

void SceneNode::SetLinearFactor( ruVector3 lin ) {
    if( body ) {
        body->setLinearFactor( btVector3( lin.x, lin.y, lin.z ) );
    }
}

ruVector3 SceneNode::GetPosition() {
    btVector3 pos = globalTransform.getOrigin();
    return ruVector3( pos.x(), pos.y(), pos.z() );
}

int SceneNode::GetContactCount() {
    return numContacts;
}

ruContact SceneNode::GetContact( int num ) {
    return contacts[ num ];
}

void SceneNode::ApplyProperties() {
    for( auto prop : properties ) {
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
            albedo = atof( value.c_str() );
        }

        if ( pname == "octree" ) {
            if ( value == "1" ) {
                for( auto mesh : meshes ) {
                    if( !mesh->octree ) {
                        // mesh->octree = new Octree( mesh, 4096 );
                    }
                }
            }
        }

        if ( pname == "visible" ) {
            visible = atoi( value.c_str());
        }

        if ( pname == "frozen" ) {
            int frozen = atoi( value.c_str());

            if( frozen ) {
                Freeze();
            }
        }

        if ( pname == "mass" )
            if( body ) {
                SetMass( atof( value.c_str()) );
            }

        if ( pname == "friction" )
            if( body ) {
                body->setFriction ( atof( value.c_str()) );
            }

        if ( pname == "hitSound" ) {
            hitSound = ruLoadSound3D( value.c_str());
            AttachSound( hitSound );
        };

        if ( pname == "idleSound" ) {
            idleSound = ruLoadSound3D( value.c_str());
            AttachSound( idleSound );
        };
    };
}


void SceneNode::AttachSound( ruSoundHandle sound ) {
    sounds.push_back( sound );
}

bool SceneNode::IsNodeInside( SceneNode * node ) {
    if( !node ) {
        return 0;
    }

    btVector3 pos = globalTransform.getOrigin();

    ruVector3 point = ruVector3( pos.x(), pos.y(), pos.z() );

    int result = 0;

    btVector3 n2Pos = node->globalTransform.getOrigin();
    for( auto mesh : node->meshes ) {
        AABB aabb = mesh->aabb;

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

    for( auto childNode : node->childs ) {
        result += childNode->IsNodeInside( this );
    }

    return result;
}

SceneNode * SceneNode::GetChild( int i ) {
    return childs[i];
}

int SceneNode::GetCountChildren() {
    return childs.size();
}

SceneNode * SceneNode::FindByName( const char * name ) {
    for( auto node : g_nodes )
        if( node->name == name ) {
            return node;
        }
    return nullptr;
}

SceneNode::~SceneNode() {
    for( auto mesh : meshes ) {
        if( mesh ) {
            delete mesh;
        }
    }

    for( auto child : childs ) {
        if( child ) {
            delete child;
        }
    }

    for( auto theNode : g_nodes ) {
        for( size_t i = 0; i < theNode->childs.size(); i++ ) {
            if( theNode->childs[i] == this ) {
                theNode->childs[i] = nullptr;
            }
        }
    }

    for( auto keyframe : keyframes ) {
        delete keyframe;
    }

    if( trimesh ) {
        delete trimesh;
    }

    if( body ) {
        if( body->getCollisionShape() ) {
            delete body->getCollisionShape();
        }

        if( body->getMotionState() ) {
            delete body->getMotionState();
        }

        g_dynamicsWorld->removeRigidBody( body );

        delete body;
    }

    if( particleEmitter ) {
        delete particleEmitter;
    }

    g_nodes.erase( find( g_nodes.begin(), g_nodes.end(), this ));
}

void SceneNode::SetFriction( float friction ) {
    if( body ) {
        body->setFriction( friction );
    }
}

void SceneNode::SetDepthHack( float depthHack ) {
    fDepthHack = depthHack;
    for( size_t i = 0; i < childs.size(); i++ ) {
        childs[ i ]->SetDepthHack( depthHack );
    }
}

void SceneNode::SetAnisotropicFriction( ruVector3 aniso ) {
    if( body ) {
        body->setAnisotropicFriction( btVector3( aniso.x, aniso.y, aniso.z ));
    }
}

void SceneNode::Move( ruVector3 speed ) {
    localTransform.setOrigin( localTransform.getOrigin() + btVector3( speed.x, speed.y, speed.z ) );

    if( body ) {
        body->setLinearVelocity(  btVector3( speed.x, speed.y, speed.z ) );
    }

    //CalculateGlobalTransform( );
}

void SceneNode::SetVelocity( ruVector3 velocity ) {
    if( body ) {
        body->setLinearVelocity( btVector3( velocity.x, velocity.y, velocity.z ) );
    }
}

void SceneNode::SetPosition( ruVector3 position ) {
    if( body ) {
        body->getWorldTransform().setOrigin( btVector3( position.x, position.y, position.z ) );
    }

    localTransform.setOrigin( btVector3( position.x, position.y, position.z ) );

    //CalculateGlobalTransform();
}

float SceneNode::GetMass() {
    if( body ) {
        return 1.0f / body->getInvMass();
    }

    return 0.0f;
}

bool SceneNode::IsFrozen() {
    return frozen;
}

void SceneNode::SetRotation( ruQuaternion rotation ) {
    if( body ) {
        body->getWorldTransform().getBasis().setRotation( btQuaternion( rotation.x, rotation.y, rotation.z, rotation.w ) );
    }

    localTransform.setRotation( btQuaternion( rotation.x, rotation.y, rotation.z, rotation.w ));

   // CalculateGlobalTransform( );
}

void SceneNode::SetBodyLocalScaling( ruVector3 scale ) {
	if( body )
	{
		body->getCollisionShape()->setLocalScaling( btVector3( scale.x, scale.y, scale.z ));
	}
}

ruVector3 SceneNode::GetLookVector() {
    btVector3 look = localTransform.getBasis().getColumn ( 2 );
    return ruVector3( look.x(), look.y(), look.z() );
}

ruVector3 SceneNode::GetAbsoluteLookVector() {
    btVector3 look = globalTransform.getBasis().getColumn ( 2 );
    return ruVector3( look.x(), look.y(), look.z() );
}

const char * SceneNode::GetName() {
    return name.c_str();
}

ruVector3 SceneNode::GetRightVector() {
    btVector3 right = localTransform.getBasis().getColumn ( 0 );
    return ruVector3( right.x(), right.y(), right.z() );
}

ruVector3 SceneNode::GetUpVector() {
    btVector3 up = localTransform.getBasis().getColumn ( 1 );
    return ruVector3( up.x(), up.y(), up.z() );
}

btTransform & SceneNode::GetGlobalTransform() {
    return globalTransform;
}

ruVector3 SceneNode::GetLocalPosition() {
    btTransform transform = localTransform;

    if( body && !frozen ) {
        transform = body->getWorldTransform();
    }

    ruVector3 lp;

    lp.x = transform.getOrigin().x();
    lp.y = transform.getOrigin().y();
    lp.z = transform.getOrigin().z();

    return lp;
}

SceneNode * SceneNode::FindInObjectByName( SceneNode * node, const char * name ) {
    if( node->name == name ) {
        return node;
    }


    for( int i = 0; i < node->childs.size(); i++ ) {
        SceneNode * child = node->childs[ i ];

        SceneNode * lookup = FindInObjectByName( child, name );

        if( lookup ) {
            return lookup;
        }
    }

    return 0;
}

void SceneNode::SetAngularVelocity( ruVector3 velocity ) {
    if( body ) {
        body->setAngularVelocity( btVector3( velocity.x, velocity.y, velocity.z ));
    }
}

ruVector3 SceneNode::GetEulerAngles() {
    float y, p, r;

    globalTransform.getBasis().getEulerYPR( y, p, r );

    y *= 180.0 / 3.14159f;
    p *= 180.0 / 3.14159f;
    r *= 180.0 / 3.14159f;

    return ruVector3( p, y, r );
}

ruQuaternion SceneNode::GetLocalRotation() {
    btTransform transform = localTransform;
    if( body ) {
        transform = body->getWorldTransform();
    }
    return ruQuaternion( transform.getRotation().x(), transform.getRotation().y(), transform.getRotation().z(), transform.getRotation().w() );
}

void SceneNode::SetDamping( float linearDamping, float angularDamping ) {
    if( body ) {
        body->setDamping( linearDamping, angularDamping );
    }
}

void SceneNode::SetGravity( const ruVector3 & gravity ) {
    btVector3 g( gravity.x, gravity.y, gravity.z );

    if(body) {
        body->setGravity( g );
    }
}

void SceneNode::SetMass( float mass ) {
    if( body ) {
        btVector3 inertia;
        body->getCollisionShape()->calculateLocalInertia( mass, inertia );
        body->setMassProps( mass, inertia );
    }
}

void SceneNode::SetBody( btRigidBody * theBody ) {
    body = theBody;
    body->activate ( true );
    body->setWorldTransform ( globalTransform );
    body->setFriction( 1 );
    body->setUserPointer( this );
    body->setRestitution( 0.0f );
    g_dynamicsWorld->addRigidBody ( body );
}

void SceneNode::SetAnimation( ruAnimation * newAnim, bool dontAffectChilds ) {
    currentAnimation = newAnim;
    if( !dontAffectChilds ) {
        for( auto child : childs ) {
            child->SetAnimation( newAnim, false );
        }
    }
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

void ruCreateOctree( ruNodeHandle node, int splitCriteria ) {
    SceneNode * n = SceneNode::CastHandle( node );

    for( int i = 0; i < n->meshes.size(); i++ ) {
        Mesh * mesh = n->meshes[ i ];

        if( mesh->octree ) {
            delete mesh->octree;
        }

        mesh->octree = new Octree( mesh, splitCriteria );
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

    for( int i = 0; i < n->meshes.size(); i++ ) {
        Mesh * mesh = n->meshes[ i ];

        if( mesh->octree ) {
            delete mesh->octree;
        }
    }
}

void ruDetachNode( ruNodeHandle node ) {
    SceneNode * n = SceneNode::CastHandle( node );

    if( n->parent ) {
        n->parent->EraseChild( n );
    }

    n->parent = 0;
}

bool ruIsNodeVisible( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->visible;
}

bool ruIsNodeInFrustum( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->inFrustum;
}

void ruAttachSound( ruSoundHandle sound, ruNodeHandle node ) {
    SceneNode::CastHandle( node )->AttachSound( sound );
}

ruNodeHandle ruCreateSceneNode( ) {
    return SceneNode::HandleFromPointer( new SceneNode );
}

ruNodeHandle ruFindInObjectByName( ruNodeHandle node, const char * name ) {
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

ruVector3 ruGetNodeAABBMax( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->GetAABBMax();
}

ruNodeHandle ruLoadScene( const char * file ) {
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

ruNodeHandle ruFindByName( const char * name ) {
    return SceneNode::HandleFromPointer( SceneNode::FindByName( name ));
}

void ruFreeSceneNode( ruNodeHandle node ) {
    delete SceneNode::CastHandle( node );
}

void ruSetNodeFriction( ruNodeHandle node, float friction ) {
    SceneNode::CastHandle( node )->SetFriction( friction );
}

void ruSetNodeAlbedo( ruNodeHandle node, float albedo ) {
    SceneNode::CastHandle( node )->albedo = albedo;
}

void ruSetNodeDepthHack( ruNodeHandle node, float depthHack ) {
    SceneNode::CastHandle( node )->SetDepthHack( depthHack );
}

void ruSetNodeAnisotropicFriction( ruNodeHandle node, ruVector3 aniso ) {
    SceneNode::CastHandle( node )->SetAnisotropicFriction( aniso );
}

bool ruIsNodeHasBody( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->body != nullptr;
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

const char * ruGetNodeName( ruNodeHandle node ) {
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
    s->localTransform.setOrigin( btVector3( pos.x, pos.y, pos.z ));
    if( s->body ) {
        s->body->getWorldTransform().setOrigin( btVector3( pos.x, pos.y, pos.z ) );
    }
    SceneNode::CastHandle( node )->CalculateGlobalTransform( );
}

void ruSetNodeLocalRotation( ruNodeHandle node, ruQuaternion rot ) {
    SceneNode * s = SceneNode::CastHandle( node );
    s->localTransform.setRotation( btQuaternion( rot.x, rot.y, rot.z, rot.w ));
    if( s->body ) {
        s->body->getWorldTransform().setRotation( btQuaternion( rot.x, rot.y, rot.z, rot.w ) );
    }
    SceneNode::CastHandle( node )->CalculateGlobalTransform( );
}

void ruSetNodeName( ruNodeHandle node, const char * name ) {
    SceneNode::CastHandle( node )->name = name;
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
    return SceneNode::CastHandle( node )->totalFrames;
}

ruAnimation * ruGetCurrentAnimation( ruNodeHandle node ) {
    return SceneNode::CastHandle( node )->currentAnimation;
}

void ruSetNodeBodyLocalScale( ruNodeHandle node, ruVector3 scale ) {
	return SceneNode::CastHandle( node )->SetBodyLocalScaling( scale );
}