#include "Precompiled.h"

#include "ItemPlace.h"

vector<ItemPlace*> ItemPlace::sItemPlaceList;

Item * ItemPlace::GetPlacedItem() {
    return pPlacedItem;
}

bool ItemPlace::PlaceItem( Item * pItem ) {
    if( pItem->GetType() == mItemTypeCanBePlaced ) {
        pPlacedItem = pItem;

		pPlayer->mInventory.ResetSelectedForUse();
		if( pItem->IsThrowable() ) {			
			pPlayer->mInventory.RemoveItem( pItem );
		}

        return true;
    }

    return false;
}

ItemPlace::ItemPlace( ruSceneNode obj, Item::Type itemTypeCanBePlaced ) {
    mObject = obj;
    pPlacedItem = nullptr;
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

ItemPlace * ItemPlace::FindByObject( ruSceneNode handle ) {
    for( auto ip : sItemPlaceList )
        if( ip->mObject == handle ) {
            return ip;
        }
    return 0;
}

ItemPlace::~ItemPlace() {
    sItemPlaceList.erase( find( sItemPlaceList.begin(), sItemPlaceList.end(), this ));
}
