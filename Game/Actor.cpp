#include "Actor.h"

void Actor::Move( ruVector3 direction, float speed ) {
	ruMoveNode( mBody, direction * speed  );
}

void Actor::Step( ruVector3 direction, float speed )
{
	// spring based step
	ruVector3 currentPosition = ruGetNodePosition( mBody );
	ruVector3 rayBegin = currentPosition;
	ruVector3 rayEnd = rayBegin - ruVector3( 0, 5, 0 );
	ruVector3 intPoint;
	ruNodeHandle rayResult = ruCastRay( rayBegin, rayEnd, &intPoint );
	ruVector3 pushpullVelocity;
	if( rayResult.IsValid() && !(rayResult == mBody) ) {
		if( ruGetContactCount( rayResult ) > 0 ) {
			pushpullVelocity.y = -( currentPosition.y - intPoint.y - mSpringLength * mCrouchMultiplier  ) * 6;
		}		
	}
	ruMoveNode( mBody, direction * speed + pushpullVelocity );
}

Actor::Actor( float height, float width ) :	mBodyHeight( height ), 
											mBodyWidth( width ), 
											mSpringLength( 1.0f ), 
											mCrouch( false ),
											mCrouchMultiplier( 1.0f )
{
    mBody = ruCreateSceneNode();
    ruSetCapsuleBody( mBody, mBodyHeight, mBodyWidth );
    ruSetAngularFactor( mBody, ruVector3( 0, 0, 0 ));
    ruSetNodeFriction( mBody, 0 );
    ruSetNodeAnisotropicFriction( mBody, ruVector3( 1, 1, 1 ));
    ruSetNodeDamping( mBody, 0, 0 );
    ruSetNodeMass( mBody, 2 );
    ruSetNodeGravity( mBody, ruVector3( 0, 0, 0 ));
}

void Actor::SetPosition( ruVector3 position ) {
    ruSetNodePosition( mBody, position );
}

Actor::~Actor() {
	ruFreeSceneNode( mBody );
}

char Actor::IsInsideZone( ruNodeHandle zone ) {
    return ruIsNodeInsideNode( mBody, zone );
}

ruVector3 Actor::GetCurrentPosition() {
    return ruGetNodePosition( mBody );
}

void Actor::StopInstant() {
    ruMoveNode( mBody, ruVector3( 0.0f, 0.0f, 0.0f ));
}

void Actor::Unfreeze() {
    ruUnfreeze( mBody );
}

void Actor::Freeze() {
    ruFreeze( mBody );
}

ruVector3 Actor::GetLookDirection() {
    return ruGetNodeLookVector( mBody );
}

void Actor::SetBodyVisible( bool state ) {
	if( state ) {
		ruShowNode( mBody );
	} else {
		ruHideNode( mBody );
	}
}

bool Actor::IsVisibleFromPoint( ruVector3 begin ) {
	return ruCastRay( begin, GetCurrentPosition(), nullptr ).pointer == mBody.pointer;
}

void Actor::Crouch( bool state ) {
	mCrouch = state;
}

void Actor::UpdateCrouch() {
	// stand up only if we can
	ruVector3 pickPoint;
	ruVector3 rayBegin = ruGetNodePosition(mBody) + ruVector3(0, mBodyHeight * mCrouchMultiplier * 1.025f, 0);
	ruVector3 rayEnd = ruGetNodePosition(mBody) + ruVector3(0, mBodyHeight * 1.05f, 0);
	ruNodeHandle upCast = ruCastRay( rayBegin, rayEnd, &pickPoint);
	if( upCast.IsValid() ) {
		if( !mCrouch ) {
			mCrouch = true;
		}
	}

	if( mCrouch ) {	
		mCrouchMultiplier -= 0.025f;	
		if( mCrouchMultiplier < 0.5f ) {
			mCrouchMultiplier = 0.5f;
		}
	} else {
		mCrouchMultiplier += 0.025f;	
		if( mCrouchMultiplier > 1.0f ) {
			mCrouchMultiplier = 1.0f;
		}
	}

	ruSetNodeBodyLocalScale( mBody, ruVector3( 1.0f, mCrouchMultiplier, 1.0f ));
}

bool Actor::IsCrouch() {
	return mCrouch;
}
