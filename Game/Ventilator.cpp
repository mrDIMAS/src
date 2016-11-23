#include "Precompiled.h"
#include "Ventilator.h"

void Ventilator::DoTurn()
{
	mAngle += mTurnSpeed;
	mBody->SetRotation(Quaternion(mTurnAxis, mAngle));
	mEngineSound->Play();
}

Ventilator::Ventilator(shared_ptr<ISceneNode> obj, float ts, Vector3 axis, shared_ptr<ISound> snd)
{
	mBody = obj;
	mTurnSpeed = ts;
	mTurnAxis = axis;
	mAngle = 0;
	mEngineSound = snd;
	mEngineSound->Attach(mBody);
}

Ventilator::~Ventilator()
{

}
