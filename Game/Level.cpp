#include "Level.h"
#include "LevelArrival.h"
#include "LevelMine.h"
#include "GUI.h"
#include "Player.h"
#include "Menu.h"
#include "LevelResearchFacility.h"
#include "TestingChamber.h"
#include "LevelIntroduction.h"
#include "SaveLoader.h"

LevelName g_initialLevel;
Level * currentLevel = 0;
LevelName Level::currentLevelName = LevelName::LUndefined;

Level::Level()
{
  music.Invalidate();
}

Level::~Level()
{
  FreeSceneNode( scene );

  for( auto item : items )
    if( !item->inInventory )
      delete item;

  for( auto sheet : sheets )
    delete sheet;

  for( auto door : doors )
    delete door;

  for( auto crawlWay : crawlWays )
    delete crawlWay;

  for( auto ladder : ladders )
    delete ladder;

  for( auto itemPlace : itemPlaces )
    delete itemPlace;

  for( auto valve : valves )
    delete valve;

  for( auto lift : lifts )
    delete lift;
}

void Level::Hide()
{
  HideNode( scene );
}

void Level::Show()
{
  ShowNode( scene );
}

void Level::Change( LevelName levelId, bool continueFromSave )
{
  Level::currentLevelName = levelId;

  static LevelName lastLevel = LevelName::LUndefined;

  if( lastLevel != Level::currentLevelName )    
  {
    DrawGUIText( menu->loc[ "loading" ].c_str(), GetResolutionWidth() / 2 - 64, GetResolutionHeight() / 2 - 64, 128, 128, gui->font, Vector3( 255, 255, 0), 1 );
    
    RenderWorld();

    lastLevel = Level::currentLevelName; 

    if( !player && Level::currentLevelName != LevelName::L0Introduction )
    {
      player = new Player();
    }

    if( currentLevel )
      delete currentLevel;

    if( Level::currentLevelName == LevelName::L0Introduction )
      currentLevel = new LevelIntroduction;
    else
      player->FreeHands();

    if( Level::currentLevelName == LevelName::L1Arrival )
      currentLevel = new LevelArrival;

    if( Level::currentLevelName == LevelName::L2Mine )
      currentLevel = new LevelMine;

    if( Level::currentLevelName == LevelName::L3ResearchFacility )
      currentLevel = new LevelResearchFacility;

    if( Level::currentLevelName == LevelName::LXTestingChamber )
      currentLevel = new TestingChamber;

    if( continueFromSave )
    {
      SaveLoader loader( "lastGame.save" );
      loader.RestoreWorldState();
    }
  }
}

void Level::AddLift( Lift * lift )
{
  lifts.push_back( lift );
}

void Level::AddValve( Valve * valve )
{
  valves.push_back( valve );
}

void Level::AddLadder( Ladder * ladder )
{
  ladders.push_back( ladder );
}

void Level::AddCrawlWay( CrawlWay * cw )
{
  crawlWays.push_back( cw );
}

void Level::AddDoor( Door * door )
{
  doors.push_back( door );
}

void Level::AddSheet( Sheet * sheet )
{
  sheets.push_back( sheet );
}

void Level::AddItem( Item * item )
{
  items.push_back( item );
}

void Level::AddItemPlace( ItemPlace * ipc )
{
  itemPlaces.push_back( ipc );
}

void Level::DeserializeWith( TextFileStream & in )
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

void Level::SerializeWith( TextFileStream & out )
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
