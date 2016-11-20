#pragma once

#include "Game.h"
#include "Item.h"
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
#include "Zone.h"
#include "ActionSeries.h"
#include "Button.h"
#include "Enemy.h"
#include "Gate.h"
#include "Keypad.h"
#include "LightSwitch.h"

// Used to transfer player state through levels
struct PlayerTransfer {
	float mHealth;
	map<Item, int> mItems;
};

class Level {
private:
	vector<shared_ptr<Door>> mDoorList;
	vector<shared_ptr<Ladder>> mLadderList;
	vector<shared_ptr<ItemPlace>> mItemPlaceList;
	vector<shared_ptr<Valve>> mValveList;
	vector<shared_ptr<Lift>> mLiftList;
	vector<shared_ptr<Zone>> mZoneList;
	vector<shared_ptr<Gate>> mGateList;
	vector<shared_ptr<Keypad>> mKeypadList;
	vector<shared_ptr<LightSwitch>> mLightSwitchList;
	vector<weak_ptr<ruSound>> mSounds;
	vector<shared_ptr<Button>> mButtonList;
	vector<shared_ptr<InteractiveObject>> mInteractiveObjectList;
	virtual void OnSerialize(SaveFile & out) = 0;
	AmbientSoundSet mAmbSoundSet;
	bool mInitializationComplete;
	int mChaseMusicStopInterval;
	float mChaseMusicVolume;
	float mDestChaseMusicVolume;
protected:
	unique_ptr<Game> & mGame;
	shared_ptr<ruSceneNode> mScene;
	ruConfig mLocalization;
	// on each level only one enemy presented	
	unique_ptr<Enemy> mEnemy;
	// on each level only one player presented
	unique_ptr<Player> mPlayer;
public:
	bool mEnded;
	explicit Level(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer);
	virtual ~Level();
	LevelName mName;
	shared_ptr<ruSound> mMusic;
	shared_ptr<ruSound> mChaseMusic;	
	void PlayChaseMusic(); // when enemy detects player, enemy should call this method
	unordered_map<string, bool > mStages;
	const unique_ptr<Enemy> & GetEnemy() const;
	const unique_ptr<Player> & GetPlayer() const;
	void DestroyPlayer();
	void AddInteractiveObject(const string & desc, const shared_ptr<InteractiveObject> & io, const ruDelegate & interactAction);
	shared_ptr<InteractiveObject> FindInteractiveObject(const string & name);
	void AddItemPlace(const shared_ptr<ItemPlace> & ipc);
	shared_ptr<ItemPlace> FindItemPlace(const string & name);
	shared_ptr<Keypad> AddKeypad(const string & keypad, const string & key0, const string & key1, const string & key2, const string & key3, const string & key4, const string & key5, const string & key6, const string & key7, const string & key8, const string & key9, const string & keyCancel, weak_ptr<Door> doorToUnlock, const string & codeToUnlock); 
	shared_ptr<Door> AddDoor(const string & nodeName, float fMaxAngle);
	shared_ptr<Door> FindDoor(const string & name);
	const vector<shared_ptr<Door>> & GetDoorList() const;
	shared_ptr<Gate> AddGate(const string & nodeName, const string & buttonOpen, const string & buttonClose, const string & buttonOpen2, const string & buttonClose2);
	const vector<shared_ptr<Gate>> & GetGateList() const;
	shared_ptr<Ladder> AddLadder(const string & hBegin, const string & hEnd, const string & hEnterZone, const string & hBeginLeavePoint, const string & hEndLeavePoint);
	shared_ptr<Ladder> FindLadder(const string & name);
	const vector<shared_ptr<Ladder>> & GetLadderList() const;
	void AddLightSwitch(const shared_ptr<LightSwitch> & lswitch);
	void AddValve(const shared_ptr<Valve> & valve);
	shared_ptr<Lift> AddLift(const string & baseNode, const string & sourceNode, const string & destNode, const string & doorFrontLeft, const string & doorFrontRight, const string & doorBackLeft, const string & mDoorBackRight);
	void AddZone(const shared_ptr<Zone> & zone);
	void AddButton(const shared_ptr<Button> & button);
	void AutoCreateDoorsByNamePattern(const string & namePattern);
	void AddSound(shared_ptr<ruSound> sound);
	void LoadLocalization(string fn);
	void AddAmbientSound(shared_ptr<ruSound> sound);
	void PlayAmbientSounds();
	void GenericUpdate();
	void LoadSceneFromFile(const string & file);
	virtual void DoScenario() = 0;
	virtual void Hide();
	virtual void Show();
	virtual bool IsVisible();
	void DoneInitialization();
	void CreateBlankScene();
	shared_ptr<ruSceneNode> GetUniqueObject(const string & name);
	static LevelName msCurLevelID;
	virtual void Serialize(SaveFile & out) final;
};