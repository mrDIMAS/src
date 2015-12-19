#include "Precompiled.h"

#include "ItemPlace.h"

vector<ItemPlace*> ItemPlace::sItemPlaceList;

Item::Type ItemPlace::GetPlacedItem() {
    return mItemPlaced;
}

bool ItemPlace::PlaceItem( Item::Type type ) {
    if( type == mItemTypeCanBePlaced ) {
        mItemPlaced = type;
		pPlayer->mInventory.ResetSelectedForUse();			
		pPlayer->mInventory.RemoveItem( type, 1 );		
        return true;
    }

    return false;
}

ItemPlace::ItemPlace( shared_ptr<ruSceneNode> obj, Item::Type itemTypeCanBePlaced ) {
    mObject = obj;
    mItemTypeCanBePlaced = itemTypeCanBePlaced;
    sItemPlaceList.push_back( this );
}

bool ItemPlace::IsPickedByPlayer() const {
    return pPlayer->mNearestPickedNode == mObject;
}

void ItemPlace::SetPlaceType( Item::Type itemTypeCanBePlaced ) {
    mItemTypeCanBePlaced = itemTypeCanBePlaced;
}

Item::Type ItemPlace::GetPlaceType() const {
    return mItemTypeCanBePlaced;
}

ItemPlace * ItemPlace::FindByObject( shared_ptr<ruSceneNode> handle ) {
    for( auto ip : sItemPlaceList )
        if( ip->mObject == handle ) {
            return ip;
        }
    return 0;
}

ItemPlace::~ItemPlace() {
    sItemPlaceList.erase( find( sItemPlaceList.begin(), sItemPlaceList.end(), this ));
}
