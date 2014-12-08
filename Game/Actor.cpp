#include "Actor.h"

void Actor::Move( ruVector3 direction, float speed ) {
	ruMoveNode( body, direction * speed );	
}

Actor::Actor( float height, float width ) {
	bodyHeight = height;
	bodyWidth = width;
	body = ruCreateSceneNode();
	ruSetCapsuleBody( body, bodyHeight, bodyWidth );
	ruSetAngularFactor( body, ruVector3( 0, 0, 0 ));
	ruSetNodeFriction( body, 0 );
	ruSetNodeAnisotropicFriction( body, ruVector3( 1, 1, 1 ));
	ruSetNodeDamping( body, 0, 0 );
	ruSetNodeMass( body, 2 );
	ruSetNodeGravity( body, ruVector3( 0, 0, 0 ));
}

void Actor::Place( ruVector3 position ) {
	ruSetNodePosition( body, position );
}

Actor::~Actor()
{

}
