/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2017 Stepanov Dmitriy aka mrDIMAS              *
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
#include "SceneNode.h"
#include "Physics.h"
#include "Camera.h"
#include "Mesh.h"
#include "Engine.h"

class FilterCallback : public btOverlapFilterCallback {
public:
	virtual bool needBroadphaseCollision( btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1 ) const {
		btCollisionObject * colObjA = static_cast<btCollisionObject*>( proxy0->m_clientObject );
		SceneNode * nodeA = static_cast<SceneNode*>( colObjA->getUserPointer( ) );

		btCollisionObject * colObjB = static_cast<btCollisionObject*>( proxy1->m_clientObject );
		SceneNode * nodeB = static_cast<SceneNode*>( colObjB->getUserPointer( ) );

		if ( !nodeA->IsCollisionEnabled( ) || !nodeB->IsCollisionEnabled( ) ) {
			return false;
		}
		if ( nodeA->IsFrozen( ) && nodeB->IsStatic( ) || nodeB->IsFrozen( ) && nodeA->IsStatic( ) ) {
			return false;
		}
		if ( nodeA->IsFrozen( ) && nodeB->IsFrozen( ) || nodeB->IsFrozen( ) && nodeA->IsFrozen( ) ) {
			return false;
		}
		if ( nodeA->IsStatic( ) && nodeB->IsStatic( ) || nodeB->IsStatic( ) && nodeA->IsStatic( ) ) {
			return false;
		}
		return true;
	}
};

void MyNearCallback( btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo ) {
	btCollisionObject * colObjA = static_cast<btCollisionObject*>( collisionPair.m_pProxy0->m_clientObject );
	SceneNode * nodeA = static_cast<SceneNode*>( colObjA->getUserPointer( ) );

	btCollisionObject * colObjB = static_cast<btCollisionObject*>( collisionPair.m_pProxy1->m_clientObject );
	SceneNode * nodeB = static_cast<SceneNode*>( colObjB->getUserPointer( ) );

	if ( !nodeA->IsCollisionEnabled( ) || !nodeB->IsCollisionEnabled( ) ) {
		return;
	}
	if ( nodeA->IsFrozen( ) && nodeB->IsStatic( ) || nodeB->IsFrozen( ) && nodeA->IsStatic( ) ) {
		return;
	}
	if ( nodeA->IsFrozen( ) && nodeB->IsFrozen( ) || nodeB->IsFrozen( ) && nodeA->IsFrozen( ) ) {
		return;
	}
	if ( nodeA->IsStatic( ) && nodeB->IsStatic( ) || nodeB->IsStatic( ) && nodeA->IsStatic( ) ) {
		return;
	}

	dispatcher.defaultNearCallback( collisionPair, dispatcher, dispatchInfo );
}

btOverlapFilterCallback * filterCallback;

Physics::Physics(Engine * engine) : mEngine(engine) {
	mpDefaultCollision = new btDefaultCollisionConfiguration();
	mpCollisionDispatcher = new btCollisionDispatcher(Physics::mpDefaultCollision);
	mpCollisionDispatcher->setNearCallback(MyNearCallback);
	mpBroadphase = new btDbvtBroadphase();
	mpSolver = new btSequentialImpulseConstraintSolver();
	mpDynamicsWorld = new btDiscreteDynamicsWorld(Physics::mpCollisionDispatcher, Physics::mpBroadphase, Physics::mpSolver, Physics::mpDefaultCollision);
	mpDynamicsWorld->setGravity(btVector3(0.f, -9.81f, 0.f));
	filterCallback = new FilterCallback();
	mpDynamicsWorld->getPairCache()->setOverlapFilterCallback(filterCallback);
}

Physics::~Physics() {
	delete filterCallback;
	delete Physics::mpDynamicsWorld;
	delete Physics::mpSolver;
	delete Physics::mpBroadphase;
	delete Physics::mpCollisionDispatcher;
	delete Physics::mpDefaultCollision;
}

Engine * Physics::GetEngine() const {
	return mEngine;
}

void Physics::UpdateContacts() {
	int numManifolds = mpDynamicsWorld->getDispatcher()->getNumManifolds();

	auto & nodes = mEngine->GetSceneFactory()->GetNodeList();
	for(auto & pWeak : nodes) {
		shared_ptr<SceneNode> & node = pWeak.lock();
		if(node) {
			if(node->mBodyList.size()) {
				node->mContactCount = 0;
			}
		}
	}

	for(int i = 0; i < numManifolds; i++) {
		btPersistentManifold* contactManifold = Physics::mpDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		SceneNode * nodeA = static_cast<SceneNode*>(obA->getUserPointer());
		SceneNode * nodeB = static_cast<SceneNode*>(obB->getUserPointer());

		if(!nodeA || !nodeB) {
			continue;
		}

		if(nodeA == nodeB) { // ???
			continue;
		}

		int numContacts = contactManifold->getNumContacts();

		if(numContacts > BODY_MAX_CONTACTS) {
			numContacts = BODY_MAX_CONTACTS;
		}

		for(int j = 0; j < numContacts; j++) {
			btManifoldPoint& pt = contactManifold->getContactPoint(j);

			if(pt.getDistance() < 0.0f) {
				nodeA->mContactCount++;
				nodeB->mContactCount++;

				int obAIndex = obA->getUserIndex();
				nodeA->mContactList[j].normal = ruVector3(pt.m_normalWorldOnB.x(), pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z());
				nodeA->mContactList[j].position = ruVector3(pt.m_positionWorldOnA.x(), pt.m_positionWorldOnA.y(), pt.m_positionWorldOnA.z());
				nodeA->mContactList[j].impulse = pt.m_appliedImpulse;
				nodeA->mContactList[j].body = nodeB;
				if(obAIndex >= 0) {
					if(nodeA->mMeshList[obAIndex]->mDiffuseTexture) {
						nodeA->mContactList[j].textureName = nodeA->mMeshList[obAIndex]->mDiffuseTexture->GetName();
					}
				}

				int obBIndex = obB->getUserIndex();
				nodeB->mContactList[j].normal = ruVector3(pt.m_normalWorldOnB.x(), pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z());
				nodeB->mContactList[j].position = ruVector3(pt.m_positionWorldOnB.x(), pt.m_positionWorldOnB.y(), pt.m_positionWorldOnB.z());
				nodeB->mContactList[j].impulse = pt.m_appliedImpulse;
				nodeB->mContactList[j].body = nodeA;
				if(obBIndex >= 0) {
					if(nodeB->mMeshList[obBIndex]->mDiffuseTexture) {
						nodeB->mContactList[j].textureName = nodeB->mMeshList[obBIndex]->mDiffuseTexture->GetName();
					}
				}

				if(pt.m_appliedImpulse > 10.0f) {
					if(!nodeA->mFrozen) {
						if(nodeA->mHitSound) {
							if(nodeA->mHitSound->pfHandle) {
								nodeA->mHitSound->Play();
							}
						}
					}
					if(!nodeB->mFrozen) {
						if(nodeB->mHitSound) {
							if(nodeB->mHitSound->pfHandle) {
								nodeB->mHitSound->Play();
							}
						}
					}
					float vol = pt.m_appliedImpulse / 20.0f;

					if(vol > 1.0f) {
						vol = 1.0f;
					}
					float pitch = 1.0f;

					if(nodeA->mHitSound) {
						if(nodeA->mHitSound->pfHandle) {
							nodeA->mHitSound->SetVolume(vol);
							nodeA->mHitSound->SetPitch(pitch);
						}
					}
					if(nodeB->mHitSound) {
						if(nodeB->mHitSound->pfHandle) {
							nodeB->mHitSound->SetVolume(vol);
							nodeB->mHitSound->SetPitch(pitch);
						}
					}
				}
			}
		}
	}
}

shared_ptr<ruSceneNode> Physics::CastRay(ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint) {
	btVector3 rayEnd = btVector3(end.x, end.y, end.z);
	btVector3 rayBegin = btVector3(begin.x, begin.y, begin.z);

	btCollisionWorld::ClosestRayResultCallback rayCallback(rayBegin, rayEnd);
	Physics::mpDynamicsWorld->rayTest(rayBegin, rayEnd, rayCallback);

	if(rayCallback.hasHit()) {
		const btRigidBody * pBody = btRigidBody::upcast(rayCallback.m_collisionObject);
		if(pBody) {
			SceneNode * node = static_cast<SceneNode*>(rayCallback.m_collisionObject->getUserPointer());

			if(node) {
				if(outPickPoint) {
					outPickPoint->x = rayCallback.m_hitPointWorld.x();
					outPickPoint->y = rayCallback.m_hitPointWorld.y();
					outPickPoint->z = rayCallback.m_hitPointWorld.z();
				};

				return std::move(node->shared_from_this());
			}
		}
	}

	return nullptr;
}

ruRayCastResultEx Physics::CastRayEx(ruVector3 begin, ruVector3 end) {
	ruRayCastResultEx result;
	result.valid = false;

	btVector3 rayEnd = btVector3(end.x, end.y, end.z);
	btVector3 rayBegin = btVector3(begin.x, begin.y, begin.z);

	btCollisionWorld::ClosestRayResultCallback rayCallback(rayBegin, rayEnd);
	Physics::mpDynamicsWorld->rayTest(rayBegin, rayEnd, rayCallback);

	if(rayCallback.hasHit()) {
		const btRigidBody * pBody = btRigidBody::upcast(rayCallback.m_collisionObject);
		if(pBody) {
			SceneNode * node = static_cast<SceneNode*>(rayCallback.m_collisionObject->getUserPointer());
			if(node) {
				result.valid = true;
				result.node = std::move(node->shared_from_this());
				result.position.x = rayCallback.m_hitPointWorld.x();
				result.position.y = rayCallback.m_hitPointWorld.y();
				result.position.z = rayCallback.m_hitPointWorld.z();
				result.normal.x = rayCallback.m_hitNormalWorld.x();
				result.normal.y = rayCallback.m_hitNormalWorld.y();
				result.normal.z = rayCallback.m_hitNormalWorld.z();
				if(node->GetMeshCount()) {
					int index = rayCallback.m_collisionObject->getUserIndex();
					result.index = index;
					if(index >= 0) {
						shared_ptr<Mesh> & mesh = node->GetMesh(index);
						if(mesh) {
							result.textureName = mesh->mDiffuseTexture->GetName();
						}
					}
				}
			}
		}
	}

	return result;
}

void Physics::Update(float timeStep, int subSteps, float fixedTimeStep) {
	Physics::mpDynamicsWorld->stepSimulation(timeStep, subSteps, fixedTimeStep);
	// grab info about node's physic contacts
	UpdateContacts();
}

inline shared_ptr<ruSceneNode> Physics::RayPick(int x, int y, ruVector3 * outPickPoint) {
	D3DVIEWPORT9 vp;
	pD3D->GetViewport(&vp);

	x *= mEngine->GetRenderer()->GetGUIWidthScaleFactor();
	y *= mEngine->GetRenderer()->GetGUIHeightScaleFactor();

	// Find screen coordinates normalized to -1,1
	D3DXVECTOR3 coord;
	coord.x = (((2.0f * x) / (float)vp.Width) - 1);
	coord.y = -(((2.0f * y) / (float)vp.Height) - 1);
	coord.z = -1.0f;

	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if(camera) {
		// Back project the ray from screen to the far clip plane
		coord.x /= camera->mProjection._11;
		coord.y /= camera->mProjection._22;

		D3DXMATRIX matinv = camera->mView;
		D3DXMatrixInverse(&matinv, NULL, &matinv);

		coord *= camera->mFarZ;
		D3DXVec3TransformCoord(&coord, &coord, &matinv);

		btVector3 rayEnd = btVector3(coord.x, coord.y, coord.z);
		btVector3 rayBegin = camera->GetGlobalTransform().getOrigin();

		btCollisionWorld::ClosestRayResultCallback rayCallback(rayBegin, rayEnd);
		Physics::mpDynamicsWorld->rayTest(rayBegin, rayEnd, rayCallback);

		if(rayCallback.hasHit()) {
			const btRigidBody * pBody = btRigidBody::upcast(rayCallback.m_collisionObject);
			if(pBody) {
				SceneNode * pNode = static_cast<SceneNode*>(pBody->getUserPointer());
				if(pNode) {
					if(outPickPoint) {
						outPickPoint->x = rayCallback.m_hitPointWorld.x();
						outPickPoint->y = rayCallback.m_hitPointWorld.y();
						outPickPoint->z = rayCallback.m_hitPointWorld.z();
					};

					return std::move(pNode->shared_from_this());
				}
			}
		}
	}
	return nullptr;
}
