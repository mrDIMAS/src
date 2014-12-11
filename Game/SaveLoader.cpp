#include "SaveLoader.h"
#include "Level.h"
#include "Way.h"

void SaveLoader::RestoreWorldState()
{
    int levNum = ReadInteger();

    // load level
    Level::Change( levNum );

    // deserialize it's objects
    if( pCurrentLevel )
        pCurrentLevel->DeserializeWith( *this );

    // deserialize items
    int itemCount = ReadInteger();

    for( int i = 0; i < itemCount; i++ ) {
        string itemName = Readstring();

        ruNodeHandle itemObject = ruFindByName( itemName.c_str());

        if( itemObject.IsValid() ) {
            Item * item = Item::GetByObject( itemObject );

            pPlayer->AddItem( item );
        }
    }

    // deserialize item places
    int countItemPlaces = ReadInteger();

    for( int i = 0; i < countItemPlaces; i++ ) {
        string ipName = Readstring();
        bool gotPlacedItem = ReadBoolean();
        string itemName;
        if( gotPlacedItem )
            itemName = Readstring();
        int placedType = ReadInteger();

        ItemPlace * pItemPlace = ItemPlace::FindByObject( ruFindByName( ipName.c_str()) );

        if( pItemPlace ) {
            Item * pItem = 0;
            if( gotPlacedItem )
                pItem = Item::GetByObject( ruFindByName( itemName.c_str() ));

            if( pItem )
                pItemPlace->pPlacedItem = pItem;

            pItemPlace->mItemTypeCanBePlaced = (Item::Type)placedType;
        }
    }

    // deserialize ways
    int wayCount = ReadInteger();
    for( int i = 0; i < wayCount; i++ ) {
        Way * way = Way::GetByObject( ruFindByName( Readstring().c_str() ));
        way->DeserializeWith( *this );
    }

    pPlayer->DeserializeWith( *this );
}

SaveLoader::~SaveLoader()
{

}

SaveLoader::SaveLoader( string fn ) : TextFileStream( fn.c_str(), false )
{

}
