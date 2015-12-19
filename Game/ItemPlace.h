#pragma once

#include "Game.h"
#include "Item.h"
#include "Player.h"

class ItemPlace {
public:
    shared_ptr<ruSceneNode> mObject;
    Item::Type mItemTypeCanBePlaced;
	Item::Type mItemPlaced;
public:
    static vector<ItemPlace*> sItemPlaceList;
    explicit ItemPlace( shared_ptr<ruSceneNode> obj, Item::Type itemTypeCanBePlaced );
    virtual ~ItemPlace();
    static ItemPlace * FindByObject( shared_ptr<ruSceneNode> handle );
    bool PlaceItem( Item::Type type );
    Item::Type GetPlacedItem( );
    bool IsPickedByPlayer() const;
    void SetPlaceType( Item::Type _itemTypeCanBePlaced );
    Item::Type GetPlaceType() const;
};