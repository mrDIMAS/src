#include "Fan.h"




void Fan::DoTurn() {
    mAngle += mTurnSpeed;
    ruSetNodeRotation( mBody, ruQuaternion( mTurnAxis, mAngle ));
    ruPlaySound( mEngineSound, 1 );
}

Fan::Fan( ruNodeHandle obj, float ts, ruVector3 axis, ruSoundHandle snd ) {
    mBody = obj;
    mTurnSpeed = ts;
    mTurnAxis = axis;
    mAngle = 0;
    mEngineSound = snd;
    ruAttachSound( mEngineSound, mBody );
}

Fan::~Fan() {

}
