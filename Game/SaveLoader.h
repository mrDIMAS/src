#pragma once

#include "Game.h"

class SaveLoader
{
private:
  ifstream file;

  string ReadString( );
  float ReadFloat();
  int ReadInteger( );
  bool ReadBoolean();
  Vector3 ReadVector3(  );
  Quaternion ReadQuaternion(  );
public:
  SaveLoader( string fn );
  ~SaveLoader();
  void RestoreWorldState( );
};