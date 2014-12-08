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
#include "AmbientSoundSet.h"

class Level {
private:
    vector<Item*> items;
    vector<Sheet*> sheets;
    vector<Door*> doors;
    vector<CrawlWay*> crawlWays;
    vector<Ladder*> ladders;
    vector<ItemPlace*> itemPlaces;
    vector<Valve*> valves;
    vector<Lift*> lifts;
    vector<ruSoundHandle> sounds;
    virtual void OnSerialize( TextFileStream & out ) = 0;
    virtual void OnDeserialize( TextFileStream & in ) = 0;
    AmbientSoundSet ambSoundSet;	
protected:
    Parser localization;
    ruNodeHandle scene;    	
public:
	int typeNum;
	ruSoundHandle music;
    map<string, bool > stages;
    void AddItem( Item * item );
    void AddItemPlace( ItemPlace * ipc );
    void AddSheet( Sheet * sheet );
    void AddDoor( Door * door );
    void AddCrawlWay( CrawlWay * cw );
    void AddLadder( Ladder * ladder );
    void AddValve( Valve * valve );
    void AddLift( Lift * lift );
    void AddSound( ruSoundHandle sound );
    void LoadLocalization( string fn );
    void AddAmbientSound( ruSoundHandle sound );
    void PlayAmbientSounds();
    explicit Level();
    virtual ~Level();
    virtual void DoScenario() = 0;
    virtual void Hide();
    virtual void Show();

    static int curLevelID;
    static void Change( int levelId, bool continueFromSave = false );

    virtual void SerializeWith( TextFileStream & out ) final;
    virtual void DeserializeWith( TextFileStream & in ) final;
};