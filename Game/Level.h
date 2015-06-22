#pragma once

#include "Game.h"
#include "Item.h"
#include "Sheet.h"
#include "Door.h"
#include "Ladder.h"
#include "ItemPlace.h"
#include "Valve.h"
#include "Lift.h"
#include "Locale.h"
#include "TextFileStream.h"
#include "Parser.h"
#include "AmbientSoundSet.h"
#include "PathFinder.h"
#include "Lamp.h"
#include "Zone.h"
#include "ActionTimer.h"

class Level {
private:
    vector<Item*> mItemList;
    vector<Sheet*> mSheetList;
    vector<Door*> mDoorList;
    vector<Ladder*> mLadderList;
    vector<ItemPlace*> mItemPlaceList;
    vector<Valve*> mValveList;
    vector<Lift*> mLiftList;
	vector<Lamp*> mLampList;
	vector<Zone*> mZoneList;
    vector<ruSoundHandle> mSounds;
    virtual void OnSerialize( TextFileStream & out ) = 0;
    virtual void OnDeserialize( TextFileStream & in ) = 0;
    AmbientSoundSet mAmbSoundSet;
    ruNodeHandle mScene;
    bool mInitializationComplete;
protected:
    Parser mLocalization;
public:
    int mTypeNum;
    ruSoundHandle mMusic;
    unordered_map<string, bool > mStages;
    void AddItem( Item * item );
    void AddItemPlace( ItemPlace * ipc );
    void AddSheet( Sheet * sheet );
    void AddDoor( Door * door );
    void AddLadder( Ladder * ladder );
    void AddValve( Valve * valve );
    void AddLift( Lift * lift );
	void AddLamp( Lamp * lamp );
	void AddZone( Zone * zone );
	void AutoCreateLampsByNamePattern( const string & namePattern, string buzzSound );
	void AutoCreateBulletsByNamePattern( const string & namePattern );
	void AutoCreateDoorsByNamePattern( const string & namePattern );
    void AddSound( ruSoundHandle sound );
    void LoadLocalization( string fn );
    void AddAmbientSound( ruSoundHandle sound );
    void PlayAmbientSounds();
	void UpdateGenericObjectsIdle();
    void LoadSceneFromFile( const string & file );
    explicit Level();
    virtual ~Level();
    virtual void DoScenario() = 0;
    virtual void Hide();
    virtual void Show();
    void DoneInitialization( );
    void BuildPath( Path & path, const string & nodeBaseName );
    void CreateBlankScene();
    ruNodeHandle GetUniqueObject( const string & name );
    static int msCurLevelID;
    static void Change( int levelId, bool continueFromSave = false );
	static ruTextHandle msGUILoadingText;
	static ruRectHandle msGUILoadingBackground;
	static ruFontHandle msGUIFont;
	static void CreateLoadingScreen();
    virtual void Serialize( TextFileStream & out ) final;
    virtual void Deserialize( TextFileStream & in ) final;
};