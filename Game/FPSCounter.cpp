#include "FPSCounter.h"




void FPSCounter::RegisterFrame()
{
  frameCounter++;

  if( GetElapsedTimeInSeconds( timer ) > 1.0f )
  {
    fps = frameCounter;

    frameCounter = 0;

    RestartTimer( timer );
  }
}

FPSCounter::FPSCounter()
{
  timer = CreateTimer();

  fps = 0;
  frameCounter = 0;
}
