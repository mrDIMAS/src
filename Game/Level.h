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
#include "SaveFile.h"
#include "Parser.h"
#include "AmbientSoundSet.h"
#include "PathFinder.h"
#include "Lamp.h"
#include "Zone.h"
#include "ActionTimer.h"
#include "ActionSeries.h"
#include "Button.h"

class Level {
private:
    vector<Item*> mItemList;
    vector<Sheet*> mSheetList;
    vector<shared_ptr<Door>> mDoorList;
    vector<shared_ptr<Ladder>> mLadderList;
    vector<shared_ptr<ItemPlace>> mItemPlaceList;
    vector<shared_ptr<Valve>> mValveList;
    vector<shared_ptr<Lift>> mLiftList;
	vector<shared_ptr<Lamp>> mLampList;
	vector<shared_ptr<Zone>> mZoneList;
    vector<ruSound> mSounds;
	vector<Button*> mButtonList;
    virtual void OnSerialize( SaveFile & out ) = 0;
    virtual void OnDeserialize( SaveFile & in ) = 0;
    AmbientSoundSet mAmbSoundSet;
    ruSceneNode mScene;
    bool mInitializationComplete;
protected:
    Parser mLocalization;
public:
    int mTypeNum;
    ruSound mMusic;
    unordered_map<string, bool > mStages;
    void AddItem( Item * item );
    void AddItemPlace( const shared_ptr<ItemPlace> & ipc );
    void AddSheet( Sheet * sheet );
    void AddDoor( const shared_ptr<Door> & door );
    void AddLadder( const shared_ptr<Ladder> & ladder );
    void AddValve( const shared_ptr<Valve> & valve );
    void AddLift( const shared_ptr<Lift> & lift );
	void AddLamp( const shared_ptr<Lamp> & lamp );
	void AddZone( const shared_ptr<Zone> & zone );
	void AddButton( Button * button );
	void AutoCreateLampsByNamePattern( const string & namePattern, string buzzSound );
	void AutoCreateBulletsByNamePattern( const string & namePattern );
	void AutoCreateDoorsByNamePattern( const string & namePattern );
    void AddSound( ruSound sound );
    void LoadLocalization( string fn );
    void AddAmbientSound( ruSound sound );
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
    ruSceneNode GetUniqueObject( const string & name );
    static int msCurLevelID;
    static void Change( int levelId, bool continueFromSave = false );
	static ruTextHandle msGUILoadingText;
	static ruRectHandle msGUILoadingBackground;
	static ruFontHandle msGUIFont;
	static void CreateLoadingScreen();
    virtual void Serialize( SaveFile & out ) final;
    virtual void Deserialize( SaveFile & in ) final;
	void SerializeAnimation( SaveFile & out, ruAnimation & anim ) {
		out.WriteInteger( anim.GetCurrentFrame() );
		out.WriteBoolean( anim.enabled );
	}
	void DeserializeAnimation( SaveFile & in, ruAnimation & anim ) {
		anim.SetCurrentFrame( in.ReadInteger());
		in.ReadBoolean( anim.enabled );
	}
};