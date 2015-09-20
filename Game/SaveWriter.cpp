#include "Precompiled.h"

#include "SaveWriter.h"
#include "Level.h"
#include "Player.h"
#include "Way.h"
#include "Enemy.h"

void SaveWriter::SaveWorldState() {	
    WriteInteger( pCurrentLevel->mTypeNum );

    if( pCurrentLevel ) {
        pCurrentLevel->Serialize( *this );
    }

    SavePlayerInventory();
    SaveItemPlaces();

    WriteInteger( Way::msWayList.size() );
    for( auto pWay : Way::msWayList ) {
        pWay->Serialize( *this );
    }

    // save player state
    pPlayer->Serialize( *this );

	// save sound playback positions
	WriteInteger( ruSound::GetCount() );
	for( int i = 0; i < ruSound::GetCount(); i++ ) {
		WriteInteger( ruSound::GetSound( i ).pfHandle );
		WriteFloat( ruSound::GetSound( i ).GetPlaybackPosition() );
	}
}

SaveWriter::~SaveWriter() {

}

SaveWriter::SaveWriter( string fn ) : SaveFile( fn, true ) {

}

void SaveWriter::SaveItemPlaces() {
    WriteInteger( ItemPlace::sItemPlaceList.size() );
    for( auto pItemPlace : ItemPlace::sItemPlaceList ) {
        WriteString( pItemPlace->mObject.GetName() );
        WriteBoolean( pItemPlace->pPlacedItem != 0 );
        if( pItemPlace->pPlacedItem ) {
            WriteString( pItemPlace->pPlacedItem->mObject.GetName() );
        }
        WriteInteger( (int)pItemPlace->GetPlaceType() );
    }
}


void SaveWriter::SavePlayerInventory() {
    if( !pPlayer ) {
        return;
    }

    pPlayer->mInventory.Serialize( *this );
}
