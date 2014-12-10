#include "ItemPlace.h"

vector<ItemPlace*> ItemPlace::sItemPlaceList;

Item * ItemPlace::GetPlacedItem()
{
    return pPlacedItem;
}

bool ItemPlace::PlaceItem( Item * item )
{
    if( item->mType == itemTypeCanBePlaced )
    {
        pPlacedItem = item;

        pPlayer->mInventory.mpItemForUse = 0;
        pPlayer->mInventory.RemoveItem( item );

        return true;
    }

    return false;
}

ItemPlace::ItemPlace( ruNodeHandle obj, int _itemTypeCanBePlaced )
{
    mObject = obj;
    pPlacedItem = nullptr;
    itemTypeCanBePlaced = _itemTypeCanBePlaced;
    sItemPlaceList.push_back( this );
}

bool ItemPlace::IsPickedByPlayer()
{
    return pPlayer->mNearestPickedNode == mObject;
}

void ItemPlace::SetPlaceType( int _itemTypeCanBePlaced )
{
    itemTypeCanBePlaced = _itemTypeCanBePlaced;
}

int ItemPlace::GetPlaceType()
{
    return itemTypeCanBePlaced;
}



ItemPlace * ItemPlace::FindByObject( ruNodeHandle handle )
{
    for( auto ip : sItemPlaceList )
        if( ip->mObject == handle )
            return ip;
    return 0;
}
