#pragma once

#include "Level.h"
#include "Lift.h"
#include "Valve.h"
#include "Enemy.h"
#include "ItemPlace.h"
#include "Ventilator.h"
#include "Sparks.h"
#include "SteamStream.h"
#include "Keypad.h"

class LevelResearchFacility : public Level {
private:
	shared_ptr<Lift> mLift1;
	shared_ptr<Lift> mLift2;
	unique_ptr<Sparks> mpPowerSparks;
	shared_ptr<Valve> mpSteamValve;
	unique_ptr<SteamStream> mpExtemeSteam;
	shared_ptr<Ventilator> mpFan1;
	shared_ptr<Ventilator> mpFan2;

	shared_ptr<ruSound> mLeverSound;
	shared_ptr<ruSound> mBurnSound;
	shared_ptr<ruSound> mSteamHissSound;

	shared_ptr<ruParticleSystem > mSteamPS;
	shared_ptr<ruParticleSystem > mThermitePS;
	shared_ptr<ruSceneNode> mExtremeSteamBlock;
	shared_ptr<ruSceneNode> mZoneExtremeSteamHurt;
	shared_ptr<ruSceneNode> fuseModel[3];
	shared_ptr<ruSceneNode> powerLever;
	shared_ptr<ruSceneNode> mPowerLeverOffModel;
	shared_ptr<ruSceneNode> mPowerLeverOnModel;
	shared_ptr<ruPointLight>mPowerLamp;
	shared_ptr<ruSceneNode> mPowerLeverSnd;
	shared_ptr<ruSceneNode> mSmallSteamPosition;

	ruAnimation mMeshAnimation;
	ruAnimation mMeshLockAnimation;

	shared_ptr<ruSceneNode> mDoorUnderFloor;
	shared_ptr<ruSceneNode> mZoneNewLevelLoad;

	shared_ptr<Keypad> mKeypad1;
	shared_ptr<Door> mKeypad1DoorToUnlock;

	shared_ptr<Keypad> mKeypad2;
	shared_ptr<Door> mKeypad2DoorToUnlock;

	shared_ptr<Keypad> mKeypad3;
	shared_ptr<Door> mKeypad3DoorToUnlock;

	shared_ptr<Keypad> mLabKeypad;
	shared_ptr<Door> mLabDoorToUnlock;

	shared_ptr<Keypad> mColliderKeypad;
	shared_ptr<Door> mColliderDoorToUnlock;

	shared_ptr<Door> mLockedDoor;

	shared_ptr<ItemPlace> mFusePlaceList[3];
	int fuseInsertedCount;
	bool mPowerOn;
	bool mSteamDisabled;

	shared_ptr<ruSceneNode> mThermiteSmall;
	shared_ptr<ruSceneNode> mThermiteBig;
	shared_ptr<ruSceneNode> mMeshLock;
	shared_ptr<ruSceneNode> mThermitePlace;
	shared_ptr<ruSceneNode> mMeshToSewers;
	shared_ptr<ruSceneNode> mEnemySpawnPosition;
	shared_ptr<ruSceneNode> mRadioHurtZone;
	ItemPlace * mThermiteItemPlace;

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
	void OnPlayerEnterDisableSteamZone() {
		if (!mStages["EnterDisableSteamZone"]) {
			mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveDisableSteam"));
			mStages["EnterDisableSteamZone"] = true;
		}
	}
public:
	explicit LevelResearchFacility(const unique_ptr<PlayerTransfer> & playerTransfer);
	virtual ~LevelResearchFacility();
	virtual void DoScenario() final;
	virtual void Hide() final;
	virtual void Show() final;
	virtual void OnSerialize(SaveFile & out) final;
};


