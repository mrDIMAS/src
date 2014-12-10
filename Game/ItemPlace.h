#pragma once

#include "Game.h"
#include "Item.h"
#include "Player.h"

class ItemPlace
{
public:
    ruNodeHandle mObject;
    int itemTypeCanBePlaced;
    Item * pPlacedItem;
public:
    static vector<ItemPlace*> sItemPlaceList;
    ItemPlace( ruNodeHandle obj, int _itemTypeCanBePlaced );
    ~ItemPlace()
    {
        sItemPlaceList.erase( find( sItemPlaceList.begin(), sItemPlaceList.end(), this ));
    }
    static ItemPlace * FindByObject( ruNodeHandle handle );
    bool PlaceItem( Item * item );
    Item * GetPlacedItem( );
    bool IsPickedByPlayer();
    void SetPlaceType( int _itemTypeCanBePlaced );
    int GetPlaceType();
};