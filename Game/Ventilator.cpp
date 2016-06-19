#include "Precompiled.h"
#include "Ventilator.h"

void Ventilator::DoTurn() {
	mAngle += mTurnSpeed;
	mBody->SetRotation(ruQuaternion(mTurnAxis, mAngle));
	mEngineSound->Play();
}

Ventilator::Ventilator(shared_ptr<ruSceneNode> obj, float ts, ruVector3 axis, shared_ptr<ruSound> snd) {
	mBody = obj;
	mTurnSpeed = ts;
	mTurnAxis = axis;
	mAngle = 0;
	mEngineSound = snd;
	mEngineSound->Attach(mBody);
}

Ventilator::~Ventilator() {

}
