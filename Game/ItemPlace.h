#pragma once

#include "Game.h"
#include "Item.h"
#include "Player.h"

class ItemPlace {
public:
    ruSceneNode mObject;
    Item::Type mItemTypeCanBePlaced;
	Item::Type mItemPlaced;
public:
    static vector<ItemPlace*> sItemPlaceList;
    explicit ItemPlace( ruSceneNode obj, Item::Type itemTypeCanBePlaced );
    virtual ~ItemPlace();
    static ItemPlace * FindByObject( ruSceneNode handle );
    bool PlaceItem( Item * item );
    Item::Type GetPlacedItem( );
    bool IsPickedByPlayer() const;
    void SetPlaceType( Item::Type _itemTypeCanBePlaced );
    Item::Type GetPlaceType() const;
};