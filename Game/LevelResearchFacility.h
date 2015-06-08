#pragma once

#include "Level.h"
#include "Lift.h"
#include "Valve.h"
#include "Enemy.h"
#include "ItemPlace.h"
#include "Fan.h"
#include "Sparks.h"
#include "SteamStream.h"
#include "Keypad.h"

class LevelResearchFacility : public Level {
private:
    Lift * mLift1;
	Lift * mLift2;
    Sparks * mpPowerSparks;
    Valve * mpSteamValve;
    SteamStream * mpExtemeSteam;
    Enemy * mpRipper;
    Fan * mpFan1;
    Fan * mpFan2;
    ruSoundHandle mFanSound;
    ruSoundHandle mLeverSound;
    ruSoundHandle mSteamHissSound;

    ruNodeHandle mSteamPS;
    ruNodeHandle mLockedDoor;
    ruNodeHandle mExtremeSteamBlock;
    ruNodeHandle mZoneExtremeSteamHurt;
    ruNodeHandle fuseModel[3];
    ruNodeHandle powerLever;
    ruNodeHandle mPowerLeverOffModel;
    ruNodeHandle mPowerLeverOnModel;
    ruNodeHandle mPowerLamp;
    ruNodeHandle mPowerLeverSnd;
    ruNodeHandle mSmallSteamPosition;
	
	
	
	ruNodeHandle mDoorUnderFloor;
	ruNodeHandle mZoneNewLevelLoad;
	ruNodeHandle mPathBlockingMesh;

	Zone * mZoneObjectiveNeedPassThroughMesh;
	Zone * mZoneEnemySpawn;
	Zone * mZoneSteamActivate;
	Zone * mZoneObjectiveRestorePower;
	Zone * mZoneExaminePlace;
	Zone * mZoneRemovePathBlockingMesh;
	Zone * mZoneNeedCrowbar;

	Keypad * mKeypad1;
	Door * mKeypad1DoorToUnlock;

	Keypad * mKeypad2;
	Door * mKeypad2DoorToUnlock;

	Keypad * mKeypad3;
	Door * mKeypad3DoorToUnlock;

	Keypad * mLabKeypad;
	Door * mLabDoorToUnlock;

	Keypad * mColliderKeypad;
	Door * mColliderDoorToUnlock;

    Item * fuse[3];
	Item * mCrowbarItem;
    ItemPlace * mFusePlaceList[3];
    int fuseInsertedCount;
    bool mPowerOn;

	ruNodeHandle mThermiteSmall;
	ruNodeHandle mThermiteBig;
	ruNodeHandle mMeshLock;
	ruNodeHandle mThermitePlace;
	ruNodeHandle mMeshToSewers;
	ruNodeHandle mEnemySpawnPosition;
	ItemPlace * mThermiteItemPlace;

	Enemy * mEnemy;

    void CreatePowerUpSequence();
    void UpdatePowerupSequence();
	void UpdateThermiteSequence();
	void CreateEnemy();

	void OnPlayerEnterSteamActivateZone();
	void OnPlayerEnterRestorePowerZone();
	void OnPlayerEnterExaminePlaceZone();
	void OnPlayerEnterRemovePathBlockingMeshZone();
	void OnPlayerEnterNeedCrowbarZone();
	void OnPlayerEnterSpawnEnemyZone();
	void OnPlayerEnterNeedPassThroughMeshZone();
	void OnCrowbarPickup();
public:
    explicit LevelResearchFacility();
    virtual ~LevelResearchFacility();
    virtual void DoScenario() final;
    virtual void Hide() final;
    virtual void Show() final;
    virtual void OnSerialize( TextFileStream & out ) final;
    virtual void OnDeserialize( TextFileStream & in ) final;
};


