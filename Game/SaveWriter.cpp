#include "Precompiled.h"

#include "SaveWriter.h"
#include "Level.h"
#include "Player.h"
#include "Way.h"
#include "Enemy.h"

void SaveWriter::SaveWorldState() {	
    if( pCurrentLevel ) {
		WriteInteger( pCurrentLevel->mTypeNum );
        pCurrentLevel->Serialize( *this );
    }

    SaveItemPlaces();
	
    WriteInteger( Way::msWayList.size() );
    for( auto pWay : Way::msWayList ) {
        pWay->Serialize( *this );
    }

    // save player state
    pPlayer->Serialize( *this );
}

SaveWriter::~SaveWriter() {

}

SaveWriter::SaveWriter( string fn ) : SaveFile( fn, true ) {

}

void SaveWriter::SaveItemPlaces() {
    WriteInteger( ItemPlace::sItemPlaceList.size() );
    for( auto pItemPlace : ItemPlace::sItemPlaceList ) {
        WriteString( pItemPlace->mObject->GetName() );
        WriteInteger( static_cast<int>( pItemPlace->mItemPlaced ));
        WriteInteger( static_cast<int>( pItemPlace->GetPlaceType()));
    }
}
