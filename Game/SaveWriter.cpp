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

    SaveItemPlaces();

    WriteInteger( Way::msWayList.size() );
    for( auto pWay : Way::msWayList ) {
        pWay->Serialize( *this );
    }

    // save player state
    pPlayer->Serialize( *this );

	// save sound playback positions
	// buggy at this moment
	/*
	WriteInteger( ruSound::GetCount() );
	for( int i = 0; i < ruSound::GetCount(); i++ ) {
		WriteInteger( ruSound::GetSound( i ).pfHandle );
		WriteFloat( ruSound::GetSound( i ).GetPlaybackPosition() );
	}*/
}

SaveWriter::~SaveWriter() {

}

SaveWriter::SaveWriter( string fn ) : SaveFile( fn, true ) {

}

void SaveWriter::SaveItemPlaces() {
    WriteInteger( ItemPlace::sItemPlaceList.size() );
    for( auto pItemPlace : ItemPlace::sItemPlaceList ) {
        WriteString( pItemPlace->mObject.GetName() );
        WriteInteger( static_cast<int>( pItemPlace->mItemPlaced ));
        WriteInteger( static_cast<int>( pItemPlace->GetPlaceType()));
    }
}
