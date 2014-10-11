#pragma once

#include "Game.h"

class Sparks
{
public:
  NodeHandle ps;
  int alive;
  SoundHandle es;
public:

  Sparks( NodeHandle at, SoundHandle emits );
  void Update();
};