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
        pCurrentLevel->Deserialize( *this );
    }

    // deserialize item places
    int countItemPlaces = ReadInteger();

    for( int i = 0; i < countItemPlaces; i++ ) {
        string ipName = ReadString();
        Item::Type placedItem = static_cast<Item::Type>( ReadInteger());
        Item::Type placeType = static_cast<Item::Type>( ReadInteger());

        ItemPlace * pItemPlace = ItemPlace::FindByObject( ruSceneNode::FindByName( ipName) );

        if( pItemPlace ) {           
            pItemPlace->mItemPlaced = placedItem;
            pItemPlace->mItemTypeCanBePlaced = placeType;
        }
    }

    // deserialize ways
    int wayCount = ReadInteger();
	cout << "way count" <<  wayCount << endl;
    for( int i = 0; i < wayCount; i++ ) {
		string name = ReadString();
		cout << name << endl;
        Way * way = Way::GetByObject( ruSceneNode::FindByName( name ));
		way->Deserialize( *this );	
    }
    pPlayer->Deserialize( *this );

	Game_UpdateClock(); 
}

SaveLoader::~SaveLoader() {

}

SaveLoader::SaveLoader( string fn ) : SaveFile( fn, false ) {

}
