#pragma once

#include "Door.h"

class LiftDoor : public Door {
private:
	float mAngleOffset;
public:
	LiftDoor(  ruNodeHandle hDoor, float fMaxAngle, float angleOffset ) : Door( hDoor, fMaxAngle ) {
		mAngleOffset = angleOffset;
	}

	virtual void DoInteraction() {
		Door::DoInteraction();

		ruSetNodeRotation( mDoorNode, ruQuaternion( ruGetNodeUpVector( mDoorNode ), mAngleOffset + mCurrentAngle ));
	}
};