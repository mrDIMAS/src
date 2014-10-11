#pragma once

#include "Game.h"
#include "ItemPlace.h"

class SaveWriter
{
private:
  ofstream file;

  void WriteString( string str )
  {
    file << str << endl;
  }

  void WriteFloat( float fl )
  {
    file << fl << endl;
  }

  void WriteInteger( int i )
  {
    file << i << endl;
  }

  void WriteBoolean( bool b )
  {
    file << b << endl;
  }

  void WriteVector3( Vector3 v )
  {
    WriteFloat( v.x );
    WriteFloat( v.y );
    WriteFloat( v.z );
    file << endl;
  }

  void WriteQuaternion( Quaternion q )
  {
    WriteFloat( q.x );
    WriteFloat( q.y );
    WriteFloat( q.z );
    WriteFloat( q.w );
    file << endl;
  }

  void SavePlayerInventory()
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

  void SaveCurrentLevelStages()
  {
    WriteInteger( currentLevel->stages.size());

    for( auto stage : currentLevel->stages )
    {
      WriteString( stage.first );
      WriteBoolean( stage.second );
    }
  }

  void SaveItemPlaces()
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
public:
  SaveWriter( string fn )
  {
    file.open( fn );
  }

  ~SaveWriter()
  {
    file.close();
  }

  void SaveWorldState( )
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
  }
};