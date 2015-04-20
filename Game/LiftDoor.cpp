#include "Precompiled.h"

#include "LiftDoor.h"

void LiftDoor::DoInteraction()
{
	Door::DoInteraction();

	ruSetNodeRotation( mDoorNode, ruQuaternion( ruGetNodeUpVector( mDoorNode ), mAngleOffset + mCurrentAngle ));
}

LiftDoor::LiftDoor( ruNodeHandle hDoor, float fMaxAngle, float angleOffset ) : Door( hDoor, fMaxAngle )
{
	mAngleOffset = angleOffset;
}
