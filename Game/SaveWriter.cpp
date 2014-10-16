#include "SaveWriter.h"
#include "Level.h"
#include "Player.h"
#include "Way.h"

void SaveWriter::SaveWorldState()
{
  if( currentLevel )
    currentLevel->SerializeWith( *this );

  SavePlayerInventory();
  SaveCurrentLevelStages();
  SaveItemPlaces();

  WriteInteger( Way::all.size() );
  for( auto way : Way::all )
    way->SerializeWith( *this );

  // save player state
  player->SerializeWith( *this );
}

SaveWriter::~SaveWriter()
{

}

SaveWriter::SaveWriter( string fn ) : TextFileStream( fn.c_str(), true )
{

}

void SaveWriter::SaveItemPlaces()
{
  WriteInteger( ItemPlace::all.size() );
  for( auto ip : ItemPlace::all )
  {
    Writestring( GetName( ip->object ));
    WriteBoolean( ip->itemPlaced != 0 );
    if( ip->itemPlaced )
      Writestring( GetName( ip->itemPlaced->object ));
    WriteInteger( ip->GetPlaceType() );
  }
}

void SaveWriter::SaveCurrentLevelStages()
{
  WriteInteger( currentLevel->stages.size());

  for( auto stage : currentLevel->stages )
  {
    Writestring( stage.first );
    WriteBoolean( stage.second );
  }
}

void SaveWriter::SavePlayerInventory()
{
  if( !player )
    return;

  WriteInteger( player->inventory.items.size() );
  for( auto item : player->inventory.items )
  {
    // write object name for further identification
    Writestring( GetName( item->object ) );
  }
}
