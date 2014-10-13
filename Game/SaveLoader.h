#pragma once

#include "Game.h"
#include "TextFileStream.h"

class SaveLoader : public TextFileStream
{
public:
  SaveLoader( string fn );
  ~SaveLoader();
  void RestoreWorldState( );
};