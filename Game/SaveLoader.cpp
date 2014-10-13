#include "SaveLoader.h"
#include "Level.h"
#include "Way.h"

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
  
  int wayCount = ReadInteger();
  for( int i = 0; i < wayCount; i++ )
  {
    Way * way = Way::GetByObject( FindByName( ReadString().c_str() ));
    way->DeserializeWith( *this );
  }

  player->DeserializeWith( *this );
}

SaveLoader::~SaveLoader()
{

}

SaveLoader::SaveLoader( string fn ) : TextFileStream( fn.c_str(), false )
{

}
