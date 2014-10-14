#pragma once

#include "Game.h"
#include "Item.h"
#include "Sheet.h"
#include "Door.h"
#include "Ladder.h"
#include "CrawlWay.h"
#include "ScreenScreamer.h"
#include "ItemPlace.h"
#include "Valve.h"
#include "Lift.h"
#include "Locale.h"
#include "TextFileStream.h"

class Level : public Locale
{
private:
  vector<Item*> items;
  vector<Sheet*> sheets;
  vector<Door*> doors;
  vector<CrawlWay*> crawlWays;
  vector<Ladder*> ladders;
  vector<ItemPlace*> itemPlaces;
  vector<Valve*> valves;
  vector<Lift*> lifts;
  
  virtual void OnSerialize( TextFileStream & out ) = 0;
  virtual void OnDeserialize( TextFileStream & in ) = 0;
public:
  map<string, bool > stages;
  void AddItem( Item * item );
  void AddItemPlace( ItemPlace * ipc );
  void AddSheet( Sheet * sheet );
  void AddDoor( Door * door );
  void AddCrawlWay( CrawlWay * cw );
  void AddLadder( Ladder * ladder );
  void AddValve( Valve * valve );
  void AddLift( Lift * lift );

  explicit Level();
  virtual ~Level();
  virtual void DoScenario() = 0;
  virtual void Hide();
  virtual void Show();
  NodeHandle scene;
  static LevelName currentLevelName;
  static void Change( LevelName levelId, bool continueFromSave = false );
  SoundHandle music;

  virtual void SerializeWith( TextFileStream & out ) final
  {
    int childCount = GetCountChildren( scene );
    out.WriteInteger( childCount );
    for( int i = 0; i < childCount; i++ )
    {
      NodeHandle node = GetChild( scene, i );
      out.WriteString( GetName( node ));
      out.WriteVector3( GetLocalPosition( node ));
      out.WriteQuaternion( GetLocalRotation( node ));
      out.WriteBoolean( IsNodeVisible( node ));
    }
    OnSerialize( out );
  }

  virtual void DeserializeWith( TextFileStream & in ) final
  {
    int childCount = in.ReadInteger( );
    for( int i = 0; i < childCount; i++ )
    {
      string name = in.ReadString();
      NodeHandle node = FindInObjectByName( scene, name.c_str() );
      SetLocalPosition( node, in.ReadVector3() );
      SetLocalRotation( node, in.ReadQuaternion() );
      bool visible = in.ReadBoolean();
      if( visible )
        ShowNode( node );
      else
        HideNode( node );
    }
    OnDeserialize( in );
  }
};