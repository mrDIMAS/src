#pragma once

#include "Common.h"

// physic world can be only one, so all members are static
class Physics {
public:
	static btDynamicsWorld * mpDynamicsWorld;
	static btDefaultCollisionConfiguration * mpDefaultCollision;
	static btCollisionDispatcher * mpCollisionDispatcher;
	static btBroadphaseInterface * mpBroadphase;
	static btSequentialImpulseConstraintSolver * mpSolver;

    static void CreateWorld();
    static void DestructWorld();
};