#pragma once

#include "Game.h"

class LightAnimator
{
public:  
  enum
  {
    LAT_PEAK_FALLOFF = 1, 
    LAT_PEAK_NORMAL,
    LAT_OFF,
    LAT_ON,
  };

  LightAnimator( NodeHandle lit, float as, float onRange, float peakRangeMult );
  void Update();
  void SetAnimationType( int lat );

  float rangeTo;
  float range;
  float peakRangeMult;
  float onRange;
  NodeHandle light;
  int lat;
  float animSpeed;


  static void UpdateAll();
};