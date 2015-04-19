#pragma once

#include "Game.h"

class Valve {
private:
	ruNodeHandle mObject;
	ruSoundHandle mTurnSound;
	ruVector3 mTurnAxis;
	float mAngle;
	bool mDone;
	int mTurnCount;
	float mValue;
public:
    explicit Valve( ruNodeHandle obj, ruVector3 axis, int turnCount = 2 );
    void Update();
	bool IsDone();
	void Reset();
	bool IsPickedByPlayer();
	float GetClosedCoeffecient();
};
