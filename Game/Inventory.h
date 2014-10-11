#pragma  once

#include "Game.h"
#include "Item.h"

class Inventory
{
public:
  vector<Item *> items;
  TextureHandle backgroundTexture;
  TextureHandle cellTexture;
  TextureHandle buttonTexture;

  bool opened;

  Item* forUse;
  Item* selected;
  Item* combineItemFirst;
  Item* combineItemSecond;

  int cellCountWidth;
  int cellCountHeight;
  int cellWidth;
  int cellHeight;

  SoundHandle pickSound;
 
  FontHandle font;

  map<string,string> localization;

  Inventory();
  void Update();
  bool IsMouseInside( int x, int y, int w, int h );
  void DoCombine();
  void RemoveItem( Item * item );
  void ThrowItem( Item * item );
};