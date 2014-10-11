#pragma once

#include "Timer.h"

class FPSCounter
{
public:
  Timer * timer;
  int frameCounter;
  int fps;

  FPSCounter();

  void RegisterFrame();
};