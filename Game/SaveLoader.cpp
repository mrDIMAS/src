#include "Precompiled.h"

#include "SaveLoader.h"
#include "Level.h"
#include "Way.h"

void SaveLoader::RestoreWorldState() {
    int levNum = ReadInteger();

    // load level
    Level::Change( levNum );

    // deserialize it's objects
    if( pCurrentLevel ) {
        pCurrentLevel->DeserializeWith( *this );
    }

    // deserialize items
    int itemCount = ReadInteger();

    for( int i = 0; i < itemCount; i++ ) {
        string itemName = ReadString();

        ruNodeHandle itemObject = ruFindByName( itemName);

        if( itemObject.IsValid() ) {
            Item * item = Item::GetItemPointerByNode( itemObject );

            pPlayer->AddItem( item );
        }
    }

    // deserialize item places
    int countItemPlaces = ReadInteger();

    for( int i = 0; i < countItemPlaces; i++ ) {
        string ipName = ReadString();
        bool gotPlacedItem = ReadBoolean();
        string itemName;
        if( gotPlacedItem ) {
            itemName = ReadString();
        }
        int placedType = ReadInteger();

        ItemPlace * pItemPlace = ItemPlace::FindByObject( ruFindByName( ipName) );

        if( pItemPlace ) {
            Item * pItem = 0;
            if( gotPlacedItem ) {
                pItem = Item::GetItemPointerByNode( ruFindByName( itemName ));
            }

            if( pItem ) {
                pItemPlace->pPlacedItem = pItem;
            }

            pItemPlace->mItemTypeCanBePlaced = (Item::Type)placedType;
        }
    }

    // deserialize ways
    int wayCount = ReadInteger();
    for( int i = 0; i < wayCount; i++ ) {
        Way * way = Way::GetByObject( ruFindByName( ReadString() ));
        way->DeserializeWith( *this );
    }

    pPlayer->DeserializeWith( *this );
}

SaveLoader::~SaveLoader() {

}

SaveLoader::SaveLoader( string fn ) : TextFileStream( fn, false ) {

}
