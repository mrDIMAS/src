#include "Actor.h"

void Actor::Move( ruVector3 direction, float speed )
{
    ruMoveNode( mBody, direction * speed );
}

Actor::Actor( float height, float width )
{
    bodyHeight = height;
    bodyWidth = width;
    mBody = ruCreateSceneNode();
    ruSetCapsuleBody( mBody, bodyHeight, bodyWidth );
    ruSetAngularFactor( mBody, ruVector3( 0, 0, 0 ));
    ruSetNodeFriction( mBody, 0 );
    ruSetNodeAnisotropicFriction( mBody, ruVector3( 1, 1, 1 ));
    ruSetNodeDamping( mBody, 0, 0 );
    ruSetNodeMass( mBody, 2 );
    ruSetNodeGravity( mBody, ruVector3( 0, 0, 0 ));
}

void Actor::SetPosition( ruVector3 position )
{
    ruSetNodePosition( mBody, position );
}

Actor::~Actor()
{

}
