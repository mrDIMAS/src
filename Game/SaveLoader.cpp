#include "SaveLoader.h"
#include "Level.h"
#include "Way.h"

void SaveLoader::RestoreWorldState() {
	int levNum = ReadInteger();
	
	Level::Change( levNum );

    if( currentLevel ) {
        currentLevel->DeserializeWith( *this );
    }

    int itemCount = ReadInteger();

    for( int i = 0; i < itemCount; i++ ) {
        string itemName = Readstring();

        ruNodeHandle itemObject = ruFindByName( itemName.c_str());

        if( itemObject.IsValid() ) {
            Item * item = Item::GetByObject( itemObject );

            player->AddItem( item );
        }
    }


    int countItemPlaces = ReadInteger();

    for( int i = 0; i < countItemPlaces; i++ ) {
        string ipName = Readstring();
        bool gotPlacedItem = ReadBoolean();
        string itemName;
        if( gotPlacedItem ) {
            itemName = Readstring();
        }
        int placedType = ReadInteger();

        ItemPlace * ip = ItemPlace::FindByObject( ruFindByName( ipName.c_str()) );

        if( ip ) {
            Item * item = 0;
            if( gotPlacedItem ) {
                item = Item::GetByObject( ruFindByName( itemName.c_str() ));
            }

            if( item ) {
                ip->itemPlaced = item;
            }

            ip->itemTypeCanBePlaced = placedType;
        }
    }

    int wayCount = ReadInteger();
    for( int i = 0; i < wayCount; i++ ) {
        Way * way = Way::GetByObject( ruFindByName( Readstring().c_str() ));
        way->DeserializeWith( *this );
    }

    player->DeserializeWith( *this );
}

SaveLoader::~SaveLoader() {

}

SaveLoader::SaveLoader( string fn ) : TextFileStream( fn.c_str(), false ) {

}
