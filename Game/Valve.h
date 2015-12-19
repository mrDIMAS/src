#pragma once

#include "Game.h"

class Valve {
private:
	shared_ptr<ruSceneNode> mObject;
	ruSound mTurnSound;
	ruVector3 mTurnAxis;
	float mAngle;
	bool mDone;
	int mTurnCount;
	float mValue;
public:
	ruEvent OnTurn;
	ruEvent OnTurnDone;
    explicit Valve( shared_ptr<ruSceneNode> obj, ruVector3 axis, int turnCount = 2 );
    void Update();
	bool IsDone();
	void Reset();
	bool IsPickedByPlayer();
	float GetClosedCoeffecient();
};
