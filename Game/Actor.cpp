#include "Actor.h"

void Actor::Move( ruVector3 direction, float speed ) {
    ruMoveNode( mBody, direction * speed );
}

Actor::Actor( float height, float width ) {
    mBodyHeight = height;
    mBodyWidth = width;
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
