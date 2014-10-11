#include "LightAnimator.h"

LightAnimator::LightAnimator( NodeHandle lit, float as, float onRange, float peakRangeMult )
{
  light = lit;
  animSpeed = as;
  this->onRange = onRange;
  this->peakRangeMult = peakRangeMult;
  lat = LAT_ON;
  range = onRange;
  rangeTo = range;
  SetLightRange( light, range );
}

void LightAnimator::Update()
{
  range = range + ( rangeTo - range ) * animSpeed;

  SetLightRange( light, range );

  if( lat == LAT_ON )
  {
    rangeTo = onRange;
  }

  if( lat == LAT_OFF )
  {
    rangeTo = 0;
  }
}

void LightAnimator::SetAnimationType( int lat )
{
  this->lat = lat;
}
