#include "SaveWriter.h"
#include "Level.h"
#include "Player.h"
#include "Way.h"

void SaveWriter::SaveWorldState()
{
  for( int i = 0; i < GetWorldObjectsCount(); i++ )
  {    
    NodeHandle node = GetWorldObject( i );
    WriteString( GetName( node ));
    WriteVector3( GetLocalPosition( node ));
    WriteQuaternion( GetLocalRotation( node ));
    WriteBoolean( IsNodeVisible( node ));
  }

  SavePlayerInventory();
  SaveCurrentLevelStages();
  SaveItemPlaces();

  Unfreeze( player->body );

  WriteInteger( Way::all.size() );
  for( auto way : Way::all )
    way->SerializeWith( *this );

  // save player state
  player->SerializeWith( *this );
}

SaveWriter::~SaveWriter()
{

}

SaveWriter::SaveWriter( string fn ) : TextFileStream( fn.c_str() )
{

}

void SaveWriter::SaveItemPlaces()
{
  WriteInteger( ItemPlace::all.size() );
  for( auto ip : ItemPlace::all )
  {
    WriteString( GetName( ip->object ));
    WriteBoolean( ip->itemPlaced != 0 );
    if( ip->itemPlaced )
      WriteString( GetName( ip->itemPlaced->object ));
    WriteInteger( ip->GetPlaceType() );
  }
}

void SaveWriter::SaveCurrentLevelStages()
{
  WriteInteger( currentLevel->stages.size());

  for( auto stage : currentLevel->stages )
  {
    WriteString( stage.first );
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
    WriteString( GetName( item->object ) );
  }
}
