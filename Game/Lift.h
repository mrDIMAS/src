#pragma once

#include "Game.h"

class Lift
{
private:
  NodeHandle body;
  NodeHandle scr;
  NodeHandle pos;
  NodeHandle sourcePoint;
  NodeHandle destPoint;
  NodeHandle target;

  bool arrived;

  SoundHandle motorIdleSound;
public:
  Lift( NodeHandle object, NodeHandle screen, NodeHandle src, NodeHandle dest, SoundHandle motorIS );
  void Update();
};