#pragma once

#include "Game.h"

class Ventilator {
private:
	shared_ptr<ISceneNode> mBody;
	float mTurnSpeed;
	Vector3 mTurnAxis;
	float mAngle;
	shared_ptr<ISound> mEngineSound;
public:
	explicit Ventilator(shared_ptr<ISceneNode> object, float turnSpeed, Vector3 turnAxis, shared_ptr<ISound> engineSound);
	virtual ~Ventilator();
	void DoTurn();
};
