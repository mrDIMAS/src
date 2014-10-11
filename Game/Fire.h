#pragma once

#include "Game.h"

class Fire
{
public:
  NodeHandle particleSystem;
  NodeHandle light;

  Fire( float size, float height );

  ~Fire();
};