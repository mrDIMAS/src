#include "Way.h"
#include "Player.h"

vector<Way*> Way::all;

Way::Way( NodeHandle hBegin, NodeHandle hEnd, NodeHandle hEnterZone )
{
  begin = hBegin;
  end = hEnd;
  enterZone = hEnterZone;
  target = hBegin;

  entering = false;
  inside = false;
  freeLook = false;

  all.push_back( this );
}

Way::~Way()
{
   all.erase( find( all.begin(), all.end(), this ));
}

void Way::Enter()
{
  inside = false;
  entering = true;
  if( ( GetPosition( player->body ) - GetPosition( begin )).Length2() < ( GetPosition( player->body ) - GetPosition( end )).Length2() )
    target = begin;
  else
    target = end;
  Freeze( player->body );
  player->currentWay = this;
}

void Way::DoEntering()
{
  if( entering )
  {
    Vector3 direction = GetPosition( target ) - GetPosition( player->body );

    float distance = direction.Length();

    direction.Normalize();

    Move( player->body, direction * 1.1f );

    if( distance < 0.25f )
    {
      entering = false;
      inside = true;

      if( target == end )
        target = begin;
      else
        target = end;

      Move( player->body, Vector3( 0, 0, 0 ));
    }
  }
}

bool Way::IsFreeLook()
{
  return freeLook;
}

NodeHandle Way::GetTarget()
{
  return target;
}

bool Way::IsEnterPicked()
{
  return player->nearestPicked == enterZone;
}

bool Way::IsPlayerInside()
{
  return inside;
}

bool Way::IsEntering()
{
  return entering;
}
