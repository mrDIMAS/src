#pragma once

#include "Game.h"

class RadioButton
{
public:

  bool on;

  RadioButton();

  void Draw( float x, float y, TextureHandle buttonImage, const char * text );
};