#include "Precompiled.h"

#include "LiftDoor.h"

void LiftDoor::DoInteraction()
{
	Door::DoInteraction();

	mDoorNode.SetRotation( ruQuaternion( mDoorNode.GetUpVector(), mAngleOffset + mCurrentAngle ));
}

LiftDoor::LiftDoor( ruSceneNode hDoor, float fMaxAngle, float angleOffset ) : Door( hDoor, fMaxAngle )
{
	mAngleOffset = angleOffset;
}
