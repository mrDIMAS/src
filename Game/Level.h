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
#include "Lamp.h"
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

class LoadingScreen {
private:
	shared_ptr<ruGUIScene> mScene;
	shared_ptr<ruText> mGUILoadingText;
	shared_ptr<ruRect> mGUILoadingBackground;
	shared_ptr<ruFont> mGUIFont;
public:
	LoadingScreen(const string & loadingText) {		
		int w = 200;
		int h = 32;
		int x = (ruVirtualScreenWidth - w) / 2;
		int y = (ruVirtualScreenHeight - h) / 2;
		mScene = ruGUIScene::Create();
		mScene->SetVisible(false);
		mGUIFont = ruFont::LoadFromFile(32, "data/fonts/font5.ttf");
		mGUILoadingText = mScene->CreateText(loadingText, x, y, w, h, mGUIFont, ruVector3(0, 0, 0), ruTextAlignment::Center);
		mGUILoadingText->SetLayer(0xFF); // topmost
		mGUILoadingBackground = mScene->CreateRect(0, 0, ruVirtualScreenWidth, ruVirtualScreenHeight, ruTexture::Request("data/gui/loadingscreen.tga"), pGUIProp->mBackColor);
	}

	void Draw() {
		mScene->SetVisible(true);
		ruEngine::HideCursor();
		ruEngine::RenderWorld();
		mScene->SetVisible(false);
	}
};

class Level {
private:
	static unique_ptr<LoadingScreen> msLoadingScreen;
	static unique_ptr<Level> msCurrent;
	vector<shared_ptr<Door>> mDoorList;
	vector<shared_ptr<Ladder>> mLadderList;
	vector<shared_ptr<ItemPlace>> mItemPlaceList;
	vector<shared_ptr<Valve>> mValveList;
	vector<shared_ptr<Lift>> mLiftList;
	vector<shared_ptr<Lamp>> mLampList;
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
	shared_ptr<ruSceneNode> mScene;
	Parser mLocalization;
	// on each level only one enemy presented	
	unique_ptr<Enemy> mEnemy;
	// on each level only one player presented
	unique_ptr<Player> mPlayer;
public:
	explicit Level(const unique_ptr<PlayerTransfer> & playerTransfer);
	virtual ~Level();

	LevelName mName;
	shared_ptr<ruSound> mMusic;
	shared_ptr<ruSound> mChaseMusic;
	// when enemy detects player, enemy should call this method
	void PlayChaseMusic();
	unordered_map<string, bool > mStages;
	unique_ptr<Enemy> & GetEnemy();
	unique_ptr<Player> & GetPlayer() {
		return mPlayer;
	}
	static void DestroyCurrent() {
		msCurrent.reset();
	}
	void AddInteractiveObject(const string & desc, const shared_ptr<InteractiveObject> & io, const ruDelegate & interactAction);
	shared_ptr<InteractiveObject> FindInteractiveObject(const string & name);
	void AddItemPlace(const shared_ptr<ItemPlace> & ipc);
	shared_ptr<ItemPlace> FindItemPlace(const string & name);
	shared_ptr<Keypad> AddKeypad(const string & keypad, const string & key0, const string & key1, const string & key2, const string & key3,
		const string & key4, const string & key5, const string & key6, const string & key7, const string & key8, const string & key9,
		const string & keyCancel, weak_ptr<Door> doorToUnlock, const string & codeToUnlock);
	shared_ptr<Door> AddDoor(const string & nodeName, float fMaxAngle);
	shared_ptr<Door> FindDoor(const string & name);
	const vector<shared_ptr<Door>> & GetDoorList() const;
	shared_ptr<Gate> AddGate(const string & nodeName, const string & buttonOpen, const string & buttonClose, const string & buttonOpen2, const string & buttonClose2);
	const vector<shared_ptr<Gate>> & GetGateList() const;
	shared_ptr<Ladder> AddLadder(const string & hBegin, const string & hEnd, const string & hEnterZone, const string & hBeginLeavePoint, const string & hEndLeavePoint);
	shared_ptr<Ladder> FindLadder(const string & name);
	const vector<shared_ptr<Ladder>> & GetLadderList() const;
	void AddLightSwitch(const shared_ptr<LightSwitch> & lswitch) {
		mLightSwitchList.push_back(lswitch);
	}
	void AddValve(const shared_ptr<Valve> & valve);
	shared_ptr<Lift> AddLift(const string & baseNode, const string & sourceNode, const string & destNode, const string & doorFrontLeft, const string & doorFrontRight, const string & doorBackLeft, const string & mDoorBackRight);
	void AddLamp(const shared_ptr<Lamp> & lamp);
	void AddZone(const shared_ptr<Zone> & zone);
	void AddButton(const shared_ptr<Button> & button);
	void AutoCreateLampsByNamePattern(const string & namePattern, string buzzSound);
	void AutoCreateBulletsByNamePattern(const string & namePattern);
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
	virtual bool IsVisible() {
		return mScene->IsVisible();
	}
	void DoneInitialization();
	void BuildPath(Path & path, const string & nodeBaseName);
	void CreateBlankScene();
	shared_ptr<ruSceneNode> GetUniqueObject(const string & name);
	static LevelName msCurLevelID;
	static void Change(LevelName name, bool continueFromSave = false);
	static void CreateLoadingScreen();
	static unique_ptr<Level> & Current();
	static void Purge();
	virtual void Serialize(SaveFile & out) final;
	//virtual void Deserialize(SaveFile & in) final;

};