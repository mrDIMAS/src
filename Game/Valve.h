#pragma once

#include "Game.h"

class Valve {
private:
	shared_ptr<ISceneNode> mObject;
	shared_ptr<ISound> mTurnSound;
	Vector3 mTurnAxis;
	float mAngle;
	bool mDone;
	int mTurnCount;
	float mValue;
public:
	Event OnTurn;
	Event OnTurnDone;
	explicit Valve(shared_ptr<ISceneNode> obj, Vector3 axis, int turnCount = 2);
	void Update();
	bool IsDone();
	void Reset();
	bool IsPickedByPlayer();
	float GetClosedCoeffecient();
};
