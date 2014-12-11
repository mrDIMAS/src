#pragma once

#include "Game.h"
#include "Item.h"
#include "Player.h"

class ItemPlace
{
public:
    ruNodeHandle mObject;
    Item::Type mItemTypeCanBePlaced;
    Item * pPlacedItem;
public:
    static vector<ItemPlace*> sItemPlaceList;
    explicit ItemPlace( ruNodeHandle obj, Item::Type itemTypeCanBePlaced );
    virtual ~ItemPlace();
    static ItemPlace * FindByObject( ruNodeHandle handle );
    bool PlaceItem( Item * item );
    Item * GetPlacedItem( );
    bool IsPickedByPlayer() const;
    void SetPlaceType( Item::Type _itemTypeCanBePlaced );
    Item::Type GetPlaceType() const;
};