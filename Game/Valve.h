#pragma once

#include "Game.h"

class Valve
{
public:
  NodeHandle object;
  Vector3 tAxis;

  float angle;
  bool done;
  int tc;

  float value;

  Valve( NodeHandle obj, Vector3 axis, int turnCount = 2 );
  void Update();
};
