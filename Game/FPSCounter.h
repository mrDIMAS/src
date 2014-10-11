#pragma once

#include "Game.h"

class FPSCounter
{
public:
  TimerHandle timer;
  int frameCounter;
  int fps;

  FPSCounter();

  void RegisterFrame();
};