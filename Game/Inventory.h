#pragma  once

#include "Game.h"
#include "Item.h"
#include "Parser.h"

class Inventory
{
public:
    vector<Item *> mItemList;
    ruTextureHandle backgroundTexture;
    ruTextureHandle cellTexture;
    ruTextureHandle buttonTexture;

    bool opened;

    Item* mpItemForUse;
    Item* selected;
    Item* combineItemFirst;
    Item* combineItemSecond;

    int cellCountWidth;
    int cellCountHeight;
    int cellWidth;
    int cellHeight;

    ruSoundHandle pickSound;

    ruFontHandle font;

    Parser localization;

    Inventory();
    void Update();
    bool IsMouseInside( int x, int y, int w, int h );
    void DoCombine();
    void RemoveItem( Item * item );
    void ThrowItem( Item * item );
};