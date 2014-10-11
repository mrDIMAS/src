#pragma once

#include "Game.h"

class Locale
{
public:
  map< string, string > loc;

  void LoadLocalizationFromFile( string fn );
};