#pragma once

#include "Game.h"
#include "Item.h"
#include "Player.h"

class ItemPlace {
public:
    NodeHandle object;
    int itemTypeCanBePlaced;
    Item * itemPlaced;
public:
    static vector<ItemPlace*> all;
    ItemPlace( NodeHandle obj, int _itemTypeCanBePlaced );
    ~ItemPlace() {
        all.erase( find( all.begin(), all.end(), this ));
    }
    static ItemPlace * FindByObject( NodeHandle handle );
    bool PlaceItem( Item * item );
    Item * GetPlacedItem( );
    bool IsPickedByPlayer();
    void SetPlaceType( int _itemTypeCanBePlaced );
    int GetPlaceType();
};