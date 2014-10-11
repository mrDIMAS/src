#pragma once

#include "Game.h"

class SaveLoader
{
private:
  ifstream file;

  string ReadString( )
  {
    string str;
    file >> str;
    return str;
  }

  float ReadFloat()
  {
    float fl;
    file >> fl;
    return fl;
  }

  int ReadInteger( )
  {
    int i;
    file >> i;
    return i;
  }

  bool ReadBoolean()
  {
    bool b;
    file >> b;
    return b;
  }

  Vector3 ReadVector3(  )
  {
    Vector3 v;
    file >> v.x;
    file >> v.y;
    file >> v.z;
    return v;
  }

  Quaternion ReadQuaternion(  )
  {
    Quaternion q;
    file >> q.x;
    file >> q.y;
    file >> q.z;
    file >> q.w;
    return q;
  }
public:
  SaveLoader( string fn )
  {
    file.open( fn );
  }

  ~SaveLoader()
  {
    file.close();
  }

  void RestoreWorldState( )
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
};