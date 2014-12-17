#pragma once

#include "Game.h"

class Valve {
private:
	ruNodeHandle mObject;
	ruVector3 mTurnAxis;
	float mAngle;
	bool mDone;
	int mTurnCount;
	float mValue;
public:
    explicit Valve( ruNodeHandle obj, ruVector3 axis, int turnCount = 2 );
    void Update();
	bool IsDone();
	float GetClosedCoeffecient();
};
