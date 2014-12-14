#include "Physics.h"

btDynamicsWorld * g_dynamicsWorld = 0;
btDefaultCollisionConfiguration * g_defaultCollision = 0;
btCollisionDispatcher * g_collisionDispatcher = 0;
btBroadphaseInterface * g_broadphase = 0;
btSequentialImpulseConstraintSolver * g_solver = 0;

void Physics::DestructWorld()
{
    if( g_dynamicsWorld )
        delete g_dynamicsWorld;
    if( g_solver )
        delete g_solver;
    if( g_broadphase )
        delete g_broadphase;
    if( g_collisionDispatcher )
        delete g_collisionDispatcher;
    if( g_defaultCollision )
        delete g_defaultCollision;
}

void Physics::CreateWorld()
{
    g_defaultCollision = new btDefaultCollisionConfiguration();
    g_collisionDispatcher = new btCollisionDispatcher ( g_defaultCollision );
    g_broadphase = new btDbvtBroadphase( );
    g_solver = new btSequentialImpulseConstraintSolver();
    g_dynamicsWorld = new btDiscreteDynamicsWorld ( g_collisionDispatcher, g_broadphase, g_solver, g_defaultCollision );
    g_dynamicsWorld->setGravity ( btVector3 ( 0.f, -9.81f,0.f ) );
}
