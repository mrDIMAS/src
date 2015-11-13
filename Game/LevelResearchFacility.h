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
	void Proxy_GiveCrowbar() {
		pPlayer->AddItem( Item::Type::Crowbar );
	}

	void Proxy_GiveFe2O3() {
		pPlayer->AddItem( Item::Type::FerrumOxide );
	}

	void Proxy_GiveAl() {
		pPlayer->AddItem( Item::Type::AluminumPowder );
	}

	void Proxy_GiveFuse() {
		pPlayer->AddItem( Item::Type::Fuse );
	}

    shared_ptr<Lift> mLift1;
	shared_ptr<Lift> mLift2;
    Sparks * mpPowerSparks;
    shared_ptr<Valve> mpSteamValve;
    SteamStream * mpExtemeSteam;
    shared_ptr<Fan> mpFan1;
    shared_ptr<Fan> mpFan2;

    ruSound mLeverSound;
	ruSound mBurnSound;
    ruSound mSteamHissSound;

    ruSceneNode mSteamPS;
    ruSceneNode mExtremeSteamBlock;
    ruSceneNode mZoneExtremeSteamHurt;
    ruSceneNode fuseModel[3];
    ruSceneNode powerLever;
    ruSceneNode mPowerLeverOffModel;
    ruSceneNode mPowerLeverOnModel;
    ruSceneNode mPowerLamp;
    ruSceneNode mPowerLeverSnd;
    ruSceneNode mSmallSteamPosition;
	
	ruAnimation mMeshAnimation;
	ruAnimation mMeshLockAnimation;
	
	ruSceneNode mDoorUnderFloor;
	ruSceneNode mZoneNewLevelLoad;

	shared_ptr<Zone> mZoneObjectiveNeedPassThroughMesh;
	shared_ptr<Zone> mZoneEnemySpawn;
	shared_ptr<Zone> mZoneSteamActivate;
	shared_ptr<Zone> mZoneObjectiveRestorePower;
	shared_ptr<Zone> mZoneExaminePlace;
	shared_ptr<Zone> mZoneRemovePathBlockingMesh;
	shared_ptr<Zone> mZoneNeedCrowbar;

	Keypad * mKeypad1;
	shared_ptr<Door> mKeypad1DoorToUnlock;

	Keypad * mKeypad2;
	shared_ptr<Door> mKeypad2DoorToUnlock;

	Keypad * mKeypad3;
	shared_ptr<Door> mKeypad3DoorToUnlock;

	Keypad * mLabKeypad;
	shared_ptr<Door> mLabDoorToUnlock;

	Keypad * mColliderKeypad;
	shared_ptr<Door> mColliderDoorToUnlock;

	shared_ptr<Door> mLockedDoor;

    shared_ptr<ItemPlace> mFusePlaceList[3];
    int fuseInsertedCount;
    bool mPowerOn;

	ruSceneNode mThermiteSmall;
	ruSceneNode mThermiteBig;
	ruSceneNode mMeshLock;
	ruSceneNode mThermitePlace;
	ruSceneNode mMeshToSewers;
	ruSceneNode mEnemySpawnPosition;
	ruSceneNode mRadioHurtZone;
	ItemPlace * mThermiteItemPlace;

	shared_ptr<Enemy> mEnemy;

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
    virtual void OnSerialize( SaveFile & out ) final;
    virtual void OnDeserialize( SaveFile & in ) final;
};


