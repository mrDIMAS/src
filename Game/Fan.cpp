#include "Precompiled.h"

#include "Fan.h"

void Fan::DoTurn() {
    mAngle += mTurnSpeed;
    mBody->SetRotation( ruQuaternion( mTurnAxis, mAngle ));
    mEngineSound.Play();
}

Fan::Fan( ruSceneNode * obj, float ts, ruVector3 axis, ruSound snd ) {
    mBody = obj;
    mTurnSpeed = ts;
    mTurnAxis = axis;
    mAngle = 0;
    mEngineSound = snd;
    mEngineSound.Attach( mBody );
}

Fan::~Fan() {
	mEngineSound.Free();
}
