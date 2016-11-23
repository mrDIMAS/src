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

#pragma once

class Engine;

class Physics : public IPhysics {
private:
	Engine * const mEngine;
public:
	btDynamicsWorld * mpDynamicsWorld;
	btDefaultCollisionConfiguration * mpDefaultCollision;
	btCollisionDispatcher * mpCollisionDispatcher;
	btBroadphaseInterface * mpBroadphase;
	btSequentialImpulseConstraintSolver * mpSolver;

	Physics(Engine * engine);
	~Physics();

	Engine * GetEngine() const;

	void UpdateContacts();

	// API Methods
	virtual shared_ptr<ISceneNode> CastRay(Vector3 begin, Vector3 end, Vector3 * outPickPoint) override final;
	virtual RayCastResultEx CastRayEx(Vector3 begin, Vector3 end) override final;
	virtual void Update(float timeStep, int subSteps, float fixedTimeStep) override final;
	virtual shared_ptr<ISceneNode> RayPick(int x, int y, Vector3 * outPickPoint) override final;
};