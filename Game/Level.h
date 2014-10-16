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
#include "Parser.h"

class Level
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
protected:
  Parser localization;
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
  void LoadLocalization( string fn );

  explicit Level();
  virtual ~Level();
  virtual void DoScenario() = 0;
  virtual void Hide();
  virtual void Show();
  NodeHandle scene;
  static int curLevelID;
  static void Change( int levelId, bool continueFromSave = false );
  SoundHandle music;

  virtual void SerializeWith( TextFileStream & out ) final;
  virtual void DeserializeWith( TextFileStream & in ) final;
};