#include "Precompiled.h"

#include "Physics.h"

btDynamicsWorld * Physics::mpDynamicsWorld = 0;
btDefaultCollisionConfiguration * Physics::mpDefaultCollision = 0;
btCollisionDispatcher * Physics::mpCollisionDispatcher = 0;
btBroadphaseInterface * Physics::mpBroadphase = 0;
btSequentialImpulseConstraintSolver * Physics::mpSolver = 0;

void Physics::DestructWorld() {
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
    Physics::mpBroadphase = new btDbvtBroadphase( );
    Physics::mpSolver = new btSequentialImpulseConstraintSolver();
    Physics::mpDynamicsWorld = new btDiscreteDynamicsWorld ( Physics::mpCollisionDispatcher, Physics::mpBroadphase, Physics::mpSolver, Physics::mpDefaultCollision );
    Physics::mpDynamicsWorld->setGravity ( btVector3 ( 0.f, -9.81f,0.f ) );
}
