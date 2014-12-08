#include "ItemPlace.h"

vector<ItemPlace*> ItemPlace::all;

Item * ItemPlace::GetPlacedItem() {
    return itemPlaced;
}

bool ItemPlace::PlaceItem( Item * item ) {
    if( item->type == itemTypeCanBePlaced ) {
        itemPlaced = item;

        player->inventory.forUse = 0;
        player->inventory.RemoveItem( item );

        return true;
    }

    return false;
}

ItemPlace::ItemPlace( ruNodeHandle obj, int _itemTypeCanBePlaced ) {
    object = obj;
    itemPlaced = nullptr;
    itemTypeCanBePlaced = _itemTypeCanBePlaced;
    all.push_back( this );
}

bool ItemPlace::IsPickedByPlayer() {
    return player->nearestPicked == object;
}

void ItemPlace::SetPlaceType( int _itemTypeCanBePlaced ) {
    itemTypeCanBePlaced = _itemTypeCanBePlaced;
}

int ItemPlace::GetPlaceType() {
    return itemTypeCanBePlaced;
}



ItemPlace * ItemPlace::FindByObject( ruNodeHandle handle ) {
    for( auto ip : all )
        if( ip->object == handle ) {
            return ip;
        }
    return 0;
}
