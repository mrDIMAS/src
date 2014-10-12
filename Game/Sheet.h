#pragma once

#include "InteractiveObject.h"

class Sheet : public InteractiveObject
{
public:
  Sheet( NodeHandle object, string desc, string text );
  void Update();
  void Draw( );
  static Sheet * GetSheetByObject( NodeHandle o );

  string txt;
  string desc;
  TextureHandle noteTex;
  static vector<Sheet*> sheets;
  static SoundHandle paperFlip;
  static FontHandle sheetFont;
};