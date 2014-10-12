#include "SaveLoader.h"
#include "Level.h"

void SaveLoader::RestoreWorldState()
{
  for( int i = 0; i < GetWorldObjectsCount(); i++ )
  {
    string nodeName = ReadString();
    Vector3 position = ReadVector3();
    Quaternion rotation = ReadQuaternion();
    bool visible = ReadBoolean();

    NodeHandle wo = GetWorldObject( i );

    if( wo.IsValid() )
    {
      SetLocalPosition( wo, position );
      SetLocalRotation( wo, rotation );
      if( visible )
        ShowNode( wo );
      else
        HideNode( wo );
    }
  }

  int itemCount = ReadInteger();

  for( int i = 0; i < itemCount; i++ )
  {
    string itemName = ReadString();

    NodeHandle itemObject = FindByName( itemName.c_str());

    if( itemObject.IsValid() )
    {
      Item * item = Item::GetByObject( itemObject );

      player->AddItem( item );
    }
  }

  int countStages = ReadInteger();

  for( int i = 0; i < countStages; i++ )
  {
    string stageName = ReadString();
    bool stageState = ReadBoolean();
    currentLevel->stages[ stageName ] = stageState;
  }

  int countItemPlaces = ReadInteger();

  for( int i = 0; i < countItemPlaces; i++ )
  {
    string ipName = ReadString();
    bool gotPlacedItem = ReadBoolean();
    string itemName;
    if( gotPlacedItem )
      itemName = ReadString();
    int placedType = ReadInteger();

    ItemPlace * ip = ItemPlace::FindByObject( FindByName( ipName.c_str()) );

    if( ip )
    {
      Item * item = 0;
      if( gotPlacedItem )
        item = Item::GetByObject( FindByName( itemName.c_str() ));

      if( item )
        ip->itemPlaced = item;

      ip->itemTypeCanBePlaced = placedType;
    }
  }
}

SaveLoader::~SaveLoader()
{
  file.close();
}

SaveLoader::SaveLoader( string fn )
{
  file.open( fn );
}

Quaternion SaveLoader::ReadQuaternion()
{
  Quaternion q;
  file >> q.x;
  file >> q.y;
  file >> q.z;
  file >> q.w;
  return q;
}

Vector3 SaveLoader::ReadVector3()
{
  Vector3 v;
  file >> v.x;
  file >> v.y;
  file >> v.z;
  return v;
}

bool SaveLoader::ReadBoolean()
{
  bool b;
  file >> b;
  return b;
}

int SaveLoader::ReadInteger()
{
  int i;
  file >> i;
  return i;
}

float SaveLoader::ReadFloat()
{
  float fl;
  file >> fl;
  return fl;
}

std::string SaveLoader::ReadString()
{
  string str;
  file >> str;
  return str;
}
