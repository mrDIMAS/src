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
    explicit ItemPlace( shared_ptr<ruSceneNode> obj, Item::Type itemTypeCanBePlaced );
    virtual ~ItemPlace();
    bool PlaceItem( Item::Type type );
    Item::Type GetPlacedItem( );
    bool IsPickedByPlayer() const;
    void SetPlaceType( Item::Type _itemTypeCanBePlaced );
    Item::Type GetPlaceType() const;
	void Serialize(SaveFile & s) {
		auto canBePlaced = (int)mItemTypeCanBePlaced;
		auto placed = (int)mItemPlaced;

		s & canBePlaced;
		s & placed;

		mItemTypeCanBePlaced = (Item::Type)canBePlaced;
		mItemPlaced = (Item::Type)placed;
	}
};