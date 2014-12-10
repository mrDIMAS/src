#include "SaveWriter.h"
#include "Level.h"
#include "Player.h"
#include "Way.h"
#include "Enemy.h"

void SaveWriter::SaveWorldState()
{
    WriteInteger( pCurrentLevel->mTypeNum );

    if( pCurrentLevel )
        pCurrentLevel->SerializeWith( *this );

    SavePlayerInventory();
    SaveItemPlaces();

    WriteInteger( Way::sWayList.size() );
    for( auto pWay : Way::sWayList )
        pWay->SerializeWith( *this );

    // save player state
    pPlayer->SerializeWith( *this );
}

SaveWriter::~SaveWriter()
{

}

SaveWriter::SaveWriter( string fn ) : TextFileStream( fn.c_str(), true )
{

}

void SaveWriter::SaveItemPlaces()
{
    WriteInteger( ItemPlace::sItemPlaceList.size() );
    for( auto pItemPlace : ItemPlace::sItemPlaceList ) {
        WriteString( ruGetNodeName( pItemPlace->mObject ));
        WriteBoolean( pItemPlace->pPlacedItem != 0 );
        if( pItemPlace->pPlacedItem )
            WriteString( ruGetNodeName( pItemPlace->pPlacedItem->mObject ));
        WriteInteger( pItemPlace->GetPlaceType() );
    }
}


void SaveWriter::SavePlayerInventory()
{
    if( !pPlayer )
        return;

    WriteInteger( pPlayer->mInventory.mItemList.size() );
    for( auto pItem : pPlayer->mInventory.mItemList ) {
        // write object name for further identification
        WriteString( ruGetNodeName( pItem->mObject ) );
    }
}
