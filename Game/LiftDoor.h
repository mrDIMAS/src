#pragma once

#include "Door.h"

class LiftDoor : public Door {
private:
	float mAngleOffset;
public:
	LiftDoor(  ruNodeHandle hDoor, float fMaxAngle, float angleOffset );

	virtual void DoInteraction();
};