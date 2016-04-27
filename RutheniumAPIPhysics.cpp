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
#include "RutheniumAPI.h"
#include "SceneNode.h"
#include "Camera.h"
#include "Renderer.h"
#include "SceneFactory.h"

shared_ptr<ruSceneNode> ruPhysics::CastRay( ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint ) {
	btVector3 rayEnd = btVector3 ( end.x, end.y, end.z );
	btVector3 rayBegin = btVector3 ( begin.x, begin.y, begin.z );

	btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
	Physics::mpDynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

	if ( rayCallback.hasHit() ) {
		const btRigidBody * pBody = btRigidBody::upcast ( rayCallback.m_collisionObject );
		if ( pBody ) {
			SceneNode * node = static_cast<SceneNode*>( rayCallback.m_collisionObject->getUserPointer());

			if ( node ) {
				if( outPickPoint ) {
					outPickPoint->x = rayCallback.m_hitPointWorld.x();
					outPickPoint->y = rayCallback.m_hitPointWorld.y();
					outPickPoint->z = rayCallback.m_hitPointWorld.z();
				};

				return std::move( node->shared_from_this() );
			}
		}
	}

	return nullptr;
}

ruRayCastResultEx ruPhysics::CastRayEx( ruVector3 begin, ruVector3 end ) {
	ruRayCastResultEx result;
	result.valid = false;

	btVector3 rayEnd = btVector3 ( end.x, end.y, end.z );
	btVector3 rayBegin = btVector3 ( begin.x, begin.y, begin.z );

	btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
	Physics::mpDynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

	if ( rayCallback.hasHit() ) {
		const btRigidBody * pBody = btRigidBody::upcast ( rayCallback.m_collisionObject );
		if ( pBody ) {
			SceneNode * node = static_cast<SceneNode*>( rayCallback.m_collisionObject->getUserPointer());
			if ( node ) {
				result.valid = true;
				result.node = std::move( node->shared_from_this());
				result.position.x = rayCallback.m_hitPointWorld.x();
				result.position.y = rayCallback.m_hitPointWorld.y();
				result.position.z = rayCallback.m_hitPointWorld.z();
				result.normal.x = rayCallback.m_hitNormalWorld.x();
				result.normal.y = rayCallback.m_hitNormalWorld.y();
				result.normal.z = rayCallback.m_hitNormalWorld.z();
				if( node->GetMeshCount() ) {
					int index = rayCallback.m_collisionObject->getUserIndex();
					result.index = index;
					if( index >= 0 ) {
						shared_ptr<Mesh> & mesh = node->GetMesh( index );
						if( mesh ) {
							result.textureName = mesh->mDiffuseTexture->GetName();
						}
					}
				}
			}
		}
	}

	return result;
}

void ruPhysics::Update( float timeStep, int subSteps, float fixedTimeStep ) {
	Physics::mpDynamicsWorld->stepSimulation( timeStep, subSteps, fixedTimeStep );
	// grab info about node's physic contacts
	SceneNode::UpdateContacts( );
}

shared_ptr<ruSceneNode> ruPhysics::RayPick( int x, int y, ruVector3 * outPickPoint ) {
	D3DVIEWPORT9 vp;
	pD3D->GetViewport( &vp );

	x *= pEngine->GetGUIWidthScaleFactor();
	y *= pEngine->GetGUIHeightScaleFactor();

	// Find screen coordinates normalized to -1,1
	D3DXVECTOR3 coord;
	coord.x = ( ( ( 2.0f * x ) / (float)vp.Width ) - 1 );
	coord.y = - ( ( ( 2.0f * y ) / (float)vp.Height ) - 1 );
	coord.z = -1.0f;

	shared_ptr<Camera> & camera = Camera::msCurrentCamera.lock();
	if( camera ) {
		// Back project the ray from screen to the far clip plane
		coord.x /= camera->mProjection._11;
		coord.y /= camera->mProjection._22;

		D3DXMATRIX matinv = camera->mView;
		D3DXMatrixInverse( &matinv, NULL, &matinv );

		coord *= camera->mFarZ;
		D3DXVec3TransformCoord ( &coord, &coord, &matinv );

		btVector3 rayEnd = btVector3 ( coord.x, coord.y, coord.z );
		btVector3 rayBegin = camera->GetGlobalTransform().getOrigin();

		btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
		Physics::mpDynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

		if ( rayCallback.hasHit() ) {
			const btRigidBody * pBody = btRigidBody::upcast ( rayCallback.m_collisionObject );
			if ( pBody ) {
				SceneNode * pNode = static_cast<SceneNode*>( pBody->getUserPointer());				 
				if ( pNode ) {
					if( outPickPoint ) {
						outPickPoint->x = rayCallback.m_hitPointWorld.x();
						outPickPoint->y = rayCallback.m_hitPointWorld.y();
						outPickPoint->z = rayCallback.m_hitPointWorld.z();
					};

					return std::move( pNode->shared_from_this() );
				}
			}
		}
	}
	return nullptr;
}