#include "Precompiled.h"
#include "Level.h"
#include "ItemPlace.h"


Item::Type ItemPlace::GetPlacedItem() {
    return mItemPlaced;
}

bool ItemPlace::PlaceItem( Item::Type type ) {
	auto & player = Level::Current()->GetPlayer();
    if( type == mItemTypeCanBePlaced ) {
        mItemPlaced = type;
		player->mInventory->ResetSelectedForUse();
		player->mInventory->RemoveItem( type, 1 );
        return true;
    }
    return false;
}

ItemPlace::ItemPlace( shared_ptr<ruSceneNode> obj, Item::Type itemTypeCanBePlaced ) :
	mItemPlaced(Item::Type::Unknown)
{
    mObject = obj;
	mItemTypeCanBePlaced = itemTypeCanBePlaced;
}

bool ItemPlace::IsPickedByPlayer() const {
	auto & player = Level::Current()->GetPlayer();
    return player->mNearestPickedNode == mObject;
}

void ItemPlace::SetPlaceType( Item::Type itemTypeCanBePlaced ) {
    mItemTypeCanBePlaced = itemTypeCanBePlaced;
}

Item::Type ItemPlace::GetPlaceType() const {
    return mItemTypeCanBePlaced;
}

ItemPlace::~ItemPlace() {

}
