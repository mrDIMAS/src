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
    for( int i = 0; i < wayCount; i++ ) {
        Way * way = Way::GetByObject( ruSceneNode::FindByName( ReadString() ));
        way->Deserialize( *this );
    }

    pPlayer->Deserialize( *this );

	// restore sound playback positions
	// buggy
	/*
	int count = ReadInteger();
	for( int i = 0; i < count; i++ ) {
		ruSound::GetSound( i ).SetPlaybackPosition( ReadFloat() );
	}*/
}

SaveLoader::~SaveLoader() {

}

SaveLoader::SaveLoader( string fn ) : SaveFile( fn, false ) {

}
