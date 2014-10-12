#pragma once

#include "Game.h"
#include "ItemPlace.h"

class SaveWriter
{
private:
  ofstream file;

  void WriteString( string str );
  void WriteFloat( float fl );
  void WriteInteger( int i );
  void WriteBoolean( bool b );
  void WriteVector3( Vector3 v );
  void WriteQuaternion( Quaternion q );
  void SavePlayerInventory();
  void SaveCurrentLevelStages();
  void SaveItemPlaces();
public:
  SaveWriter( string fn );
  ~SaveWriter();
  void SaveWorldState( );
};