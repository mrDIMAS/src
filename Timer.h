#pragma once

#include "Common.h"

class Timer
{ 
public:
  LARGE_INTEGER freq;
  double lastTime;

  Timer();
  void RestartTimer( );
  double GetTimeInSeconds( );
  double GetTimeInMilliSeconds( );
  double GetTimeInMicroSeconds( );
  double GetElapsedTimeInSeconds( );
  double GetElapsedTimeInMilliSeconds( );
  double GetElapsedTimeInMicroSeconds( );
};