#include "Fan.h"




void Fan::DoTurn()
{
  angle += turnSpeed;
  SetRotation( body, Quaternion( tAxis, angle )); 
  PlaySoundSource( sound, 1 );
}

Fan::Fan( NodeHandle obj, float ts, Vector3 axis, SoundHandle snd )
{
  body = obj;
  turnSpeed = ts;
  tAxis = axis;
  angle = 0;
  sound = snd;
  AttachSound( sound, body );
}
