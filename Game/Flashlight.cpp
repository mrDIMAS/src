#include "Flashlight.h"




void Flashlight::Update()
{
  if( on )
  {
    rangeDest = onRange;

    float time = GetElapsedTimeInSeconds( chargeTimer );

    charge = 1.0f - maxCharge * ( time ) / chargeWorkTimeSeconds;    
    
    if( charge < 0.25f )
      charge = 0.25f;
  }
  else
  {
    rangeDest = 0.0f; 

    RestartTimer( chargeTimer );
  }

  realRange += ( rangeDest - realRange ) * 0.15f;

  SetLightRange( light, realRange * charge );

  position = position.Lerp( destPosition, 0.15f );

  SetPosition( model, position );
}

void Flashlight::Switch()
{
  if( on )
    SwitchOff();
  else
    SwitchOn();
}

void Flashlight::SwitchOn()
{
  if( !on )
  {
    PlaySoundSource( onSound );

    on = true;   

    destPosition = initialPosition;
  }
}

void Flashlight::SwitchOff()
{
  if( on )
  {
    PlaySoundSource( offSound );

    on = false;    

    destPosition = Vector3( -1.0f, -1.0f, -1.0f );

    offTime += GetElapsedTimeInSeconds( chargeTimer );
  }
}

void Flashlight::Attach( NodeHandle node )
{
  ::Attach( model, node );

  initialPosition = GetPosition( model );
  destPosition = initialPosition;
  position = initialPosition;
}

void Flashlight::Fuel()
{
  charge = maxCharge;
  offTime = 0.0f;
  RestartTimer( chargeTimer );
}

bool Flashlight::GotCharge()
{
  return charge > 0.0f;
}

Flashlight::Flashlight()
{
  model = LoadScene( "data/models/hands/arm.scene" );

  light = FindInObjectByName( model, "PlayerLight" );

  onSound = CreateSound2D( "data/sounds/flashlight/on.ogg" );
  offSound = CreateSound2D( "data/sounds/flashlight/off.ogg" );
  outOfChargeSound = CreateSound2D( "data/sounds/flashlight/outofcharge.ogg" );

  chargeTimer = CreateTimer();

  chargeWorkTimeSeconds = 360.0f;

  onRange = GetLightRange( light );

  realRange = onRange;
  rangeDest = onRange;

  maxCharge = 1.0f;
  charge = maxCharge;

  offTime = 0.0f;

  on = true;
}

void Flashlight::DeserializeWith( TextFileStream & in )
{
  in.ReadFloat( maxCharge );
  in.ReadFloat( charge );
  in.ReadFloat( onRange );
  in.ReadFloat( realRange );
  in.ReadFloat( rangeDest );
  in.ReadFloat( chargeWorkTimeSeconds );
  in.ReadFloat( offTime );
  in.ReadBoolean( on );
  if( on )
    SwitchOn();
  else
    SwitchOff();
}

void Flashlight::SerializeWith( TextFileStream & out )
{
  out.WriteFloat( maxCharge );
  out.WriteFloat( charge );
  out.WriteFloat( onRange );
  out.WriteFloat( realRange );
  out.WriteFloat( rangeDest );
  out.WriteFloat( chargeWorkTimeSeconds );
  out.WriteFloat( offTime );
  out.WriteBoolean( on );
}
