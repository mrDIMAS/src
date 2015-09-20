#pragma once

#include "Game.h"

class Valve {
private:
	ruSceneNode mObject;
	ruSound mTurnSound;
	ruVector3 mTurnAxis;
	float mAngle;
	bool mDone;
	int mTurnCount;
	float mValue;
public:
	ruEvent OnTurn;
	ruEvent OnTurnDone;
    explicit Valve( ruSceneNode obj, ruVector3 axis, int turnCount = 2 );
    void Update();
	bool IsDone();
	void Reset();
	bool IsPickedByPlayer();
	float GetClosedCoeffecient();
};
