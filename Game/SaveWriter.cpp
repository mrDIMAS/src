#include "SaveWriter.h"
#include "Level.h"
#include "Player.h"
#include "Way.h"
#include "Enemy.h"

void SaveWriter::SaveWorldState() {
    WriteInteger( pCurrentLevel->mTypeNum );

    if( pCurrentLevel ) {
        pCurrentLevel->SerializeWith( *this );
    }

    SavePlayerInventory();
    SaveItemPlaces();

    WriteInteger( Way::msWayList.size() );
    for( auto pWay : Way::msWayList ) {
        pWay->SerializeWith( *this );
    }

    // save player state
    pPlayer->SerializeWith( *this );
}

SaveWriter::~SaveWriter() {

}

SaveWriter::SaveWriter( string fn ) : TextFileStream( fn, true ) {

}

void SaveWriter::SaveItemPlaces() {
    WriteInteger( ItemPlace::sItemPlaceList.size() );
    for( auto pItemPlace : ItemPlace::sItemPlaceList ) {
        WriteString( ruGetNodeName( pItemPlace->mObject ));
        WriteBoolean( pItemPlace->pPlacedItem != 0 );
        if( pItemPlace->pPlacedItem ) {
            WriteString( ruGetNodeName( pItemPlace->pPlacedItem->mObject ));
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
