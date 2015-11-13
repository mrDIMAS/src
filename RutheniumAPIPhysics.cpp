#include "Precompiled.h"
#include "Physics.h"
#include "RutheniumAPI.h"
#include "SceneNode.h"
#include "Camera.h"
#include "Engine.h"

ruSceneNode ruCastRay( ruVector3 begin, ruVector3 end, ruVector3 * outPickPoint ) {
	btVector3 rayEnd = btVector3 ( end.x, end.y, end.z );
	btVector3 rayBegin = btVector3 ( begin.x, begin.y, begin.z );

	btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
	Physics::mpDynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

	if ( rayCallback.hasHit() ) {
		const btRigidBody * pBody = btRigidBody::upcast ( rayCallback.m_collisionObject );
		if ( pBody ) {
			SceneNode * node = (SceneNode*)rayCallback.m_collisionObject->getUserPointer();

			if ( node ) {
				if( outPickPoint ) {
					outPickPoint->x = rayCallback.m_hitPointWorld.x();
					outPickPoint->y = rayCallback.m_hitPointWorld.y();
					outPickPoint->z = rayCallback.m_hitPointWorld.z();
				};

				return SceneNode::HandleFromPointer( node );
			}
		}
	}

	return SceneNode::HandleFromPointer( 0 );
}

ruRayCastResultEx ruCastRayEx( ruVector3 begin, ruVector3 end ) {
	ruRayCastResultEx result;
	result.valid = false;

	btVector3 rayEnd = btVector3 ( end.x, end.y, end.z );
	btVector3 rayBegin = btVector3 ( begin.x, begin.y, begin.z );

	btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
	Physics::mpDynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

	if ( rayCallback.hasHit() ) {
		const btRigidBody * pBody = btRigidBody::upcast ( rayCallback.m_collisionObject );
		if ( pBody ) {
			SceneNode * node = (SceneNode*)rayCallback.m_collisionObject->getUserPointer();
			if ( node ) {
				result.valid = true;
				result.node = SceneNode::HandleFromPointer( node );
				result.position.x = rayCallback.m_hitPointWorld.x();
				result.position.y = rayCallback.m_hitPointWorld.y();
				result.position.z = rayCallback.m_hitPointWorld.z();
				result.normal.x = rayCallback.m_hitNormalWorld.x();
				result.normal.y = rayCallback.m_hitNormalWorld.y();
				result.normal.z = rayCallback.m_hitNormalWorld.z();
				if( node->mMeshList.size() ) {
					int index = rayCallback.m_collisionObject->getUserIndex();
					result.index = index;
					if( index >= 0 ) {
						try {
							result.textureName = node->mMeshList.at( index )->mDiffuseTexture->GetName();
						} catch( ... ) {
							// do nothing
						}
					}
				}
			}
		}
	}

	return result;
}

ruSceneNode ruRayPick( int x, int y, ruVector3 * outPickPoint ) {
	D3DVIEWPORT9 vp;
	Engine::Instance().GetDevice()->GetViewport( &vp );
	// Find screen coordinates normalized to -1,1
	D3DXVECTOR3 coord;
	coord.x = ( ( ( 2.0f * x ) / (float)vp.Width ) - 1 );
	coord.y = - ( ( ( 2.0f * y ) / (float)vp.Height ) - 1 );
	coord.z = -1.0f;

	// Back project the ray from screen to the far clip plane
	coord.x /= Camera::msCurrentCamera->mProjection._11;
	coord.y /= Camera::msCurrentCamera->mProjection._22;

	D3DXMATRIX matinv = Camera::msCurrentCamera->mView;
	D3DXMatrixInverse( &matinv, NULL, &matinv );

	coord *= Camera::msCurrentCamera->mFarZ;
	D3DXVec3TransformCoord ( &coord, &coord, &matinv );

	btVector3 rayEnd = btVector3 ( coord.x, coord.y, coord.z );
	btVector3 rayBegin = Camera::msCurrentCamera->mGlobalTransform.getOrigin();

	btCollisionWorld::ClosestRayResultCallback rayCallback ( rayBegin, rayEnd );
	Physics::mpDynamicsWorld->rayTest ( rayBegin, rayEnd, rayCallback );

	if ( rayCallback.hasHit() ) {
		const btRigidBody * pBody = btRigidBody::upcast ( rayCallback.m_collisionObject );
		if ( pBody ) {
			SceneNode * node = ( SceneNode * ) pBody->getUserPointer();

			if ( node ) {
				if( outPickPoint ) {
					outPickPoint->x = rayCallback.m_hitPointWorld.x();
					outPickPoint->y = rayCallback.m_hitPointWorld.y();
					outPickPoint->z = rayCallback.m_hitPointWorld.z();
				};

				return SceneNode::HandleFromPointer( node );
			}
		}
	}

	return SceneNode::HandleFromPointer( 0 );
}