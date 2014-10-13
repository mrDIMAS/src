#pragma once

#include "TextFileStream.h"

class SaveManager
{
public:
  TextFileStream * stream;

  void RestoreWorldState( const char * fromFile )
  {
    stream = new TextFileStream( fromFile );

    delete stream;
  }

  void SaveWorldState()
  {
    stream = new TextFileStream( fromFile );

    delete stream;
  }
};