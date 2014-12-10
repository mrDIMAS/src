#include "Fan.h"




void Fan::DoTurn()
{
    angle += turnSpeed;
    ruSetNodeRotation( body, ruQuaternion( tAxis, angle ));
    ruPlaySound( sound, 1 );
}

Fan::Fan( ruNodeHandle obj, float ts, ruVector3 axis, ruSoundHandle snd )
{
    body = obj;
    turnSpeed = ts;
    tAxis = axis;
    angle = 0;
    sound = snd;
    ruAttachSound( sound, body );
}
