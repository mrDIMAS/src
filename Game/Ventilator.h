#pragma once

#include "Game.h"

class Ventilator {
private:
	shared_ptr<ruSceneNode> mBody;
	float mTurnSpeed;
	ruVector3 mTurnAxis;
	float mAngle;
	shared_ptr<ruSound> mEngineSound;
public:
	explicit Ventilator(shared_ptr<ruSceneNode> object, float turnSpeed, ruVector3 turnAxis, shared_ptr<ruSound> engineSound);
	virtual ~Ventilator();
	void DoTurn();
};
