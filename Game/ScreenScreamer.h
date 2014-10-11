#pragma  once

#include "game.h"

class ScreenScreamer
{
public:
  SoundHandle sound;
  int periods;
  TextureHandle tex;
  int nextPeriodFrames;
  int pauseFrames;
  ScreenScreamer();
  void DoPeriods( int p );
  void Update( );
};