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
#include "SceneNode.h"
#include "Physics.h"

btDynamicsWorld * Physics::mpDynamicsWorld = 0;
btDefaultCollisionConfiguration * Physics::mpDefaultCollision = 0;
btCollisionDispatcher * Physics::mpCollisionDispatcher = 0;
btBroadphaseInterface * Physics::mpBroadphase = 0;
btSequentialImpulseConstraintSolver * Physics::mpSolver = 0;


class FilterCallback : public btOverlapFilterCallback {
public:
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0,btBroadphaseProxy* proxy1) const	{
		btCollisionObject * colObjA = static_cast<btCollisionObject*>( proxy0->m_clientObject );
		SceneNode * nodeA = static_cast<SceneNode*>( colObjA->getUserPointer());

		btCollisionObject * colObjB = static_cast<btCollisionObject*>( proxy1->m_clientObject );
		SceneNode * nodeB = static_cast<SceneNode*>( colObjB->getUserPointer());

		if( nodeA->IsFrozen() && nodeB->IsStatic() || nodeB->IsFrozen() && nodeA->IsStatic() ) {
			return false;
		} 
		if( nodeA->IsFrozen() && nodeB->IsFrozen() || nodeB->IsFrozen() && nodeA->IsFrozen() ) {
			return false;
		} 
		if( nodeA->IsStatic() && nodeB->IsStatic() || nodeB->IsStatic() && nodeA->IsStatic() ) {
			return false;
		}
		return true;		
	}
};

void MyNearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo) {
	// Do your collision logic here
	// Only dispatch the Bullet collision information if you want the physics to continue
	btCollisionObject * colObjA = static_cast<btCollisionObject*>( collisionPair.m_pProxy0->m_clientObject );
	SceneNode * nodeA = static_cast<SceneNode*>( colObjA->getUserPointer());

	btCollisionObject * colObjB = static_cast<btCollisionObject*>( collisionPair.m_pProxy1->m_clientObject );
	SceneNode * nodeB = static_cast<SceneNode*>( colObjB->getUserPointer());

	if( nodeA->IsFrozen() && nodeB->IsStatic() || nodeB->IsFrozen() && nodeA->IsStatic() ) {
		return;
	} 
	if( nodeA->IsFrozen() && nodeB->IsFrozen() || nodeB->IsFrozen() && nodeA->IsFrozen() ) {
		return;
	} 
	if( nodeA->IsStatic() && nodeB->IsStatic() || nodeB->IsStatic() && nodeA->IsStatic() ) {
		return;
	}

	dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
}

btOverlapFilterCallback * filterCallback;

void Physics::DestructWorld() {
	delete filterCallback;

    if( Physics::mpDynamicsWorld ) {
        delete Physics::mpDynamicsWorld;
    }
    if( Physics::mpSolver ) {
        delete Physics::mpSolver;
    }
    if( Physics::mpBroadphase ) {
        delete Physics::mpBroadphase;
    }
    if( Physics::mpCollisionDispatcher ) {
        delete Physics::mpCollisionDispatcher;
    }
    if( Physics::mpDefaultCollision ) {
        delete Physics::mpDefaultCollision;
    }
}

void Physics::CreateWorld() {
    Physics::mpDefaultCollision = new btDefaultCollisionConfiguration();
    Physics::mpCollisionDispatcher = new btCollisionDispatcher ( Physics::mpDefaultCollision );
	Physics::mpCollisionDispatcher ->setNearCallback(MyNearCallback) ;
    Physics::mpBroadphase = new btDbvtBroadphase( );
    Physics::mpSolver = new btSequentialImpulseConstraintSolver();
    Physics::mpDynamicsWorld = new btDiscreteDynamicsWorld ( Physics::mpCollisionDispatcher, Physics::mpBroadphase, Physics::mpSolver, Physics::mpDefaultCollision );
    Physics::mpDynamicsWorld->setGravity ( btVector3 ( 0.f, -9.81f,0.f ) );
	filterCallback = new FilterCallback();
	Physics::mpDynamicsWorld->getPairCache()->setOverlapFilterCallback(filterCallback);
}
