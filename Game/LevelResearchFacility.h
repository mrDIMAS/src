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

	shared_ptr<ISound> mBurnSound;
	shared_ptr<ISound> mSteamHissSound;

	shared_ptr<IParticleSystem > mSteamPS;
	shared_ptr<IParticleSystem > mThermitePS;
	shared_ptr<ISceneNode> mExtremeSteamBlock;
	shared_ptr<ISceneNode> mZoneExtremeSteamHurt;
	shared_ptr<ISceneNode> fuseModel[3];
	shared_ptr<ISceneNode> powerLever;
	shared_ptr<ISceneNode> mSmallSteamPosition;

	shared_ptr<ISound> mInsertSound;
	shared_ptr<ISound> mSwitchSound;
	Animation mPowerSwitchAnimation;

	Animation mMeshAnimation;
	Animation mMeshLockAnimation;

	shared_ptr<ISceneNode> mDoorUnderFloor;
	shared_ptr<ISceneNode> mZoneNewLevelLoad;

	shared_ptr<Keypad> mKeypad1;
	shared_ptr<Door> mKeypad1DoorToUnlock;

	shared_ptr<Keypad> mKeypad2;
	shared_ptr<Door> mKeypad2DoorToUnlock;

	shared_ptr<IFog> mFog;

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

	shared_ptr<ISceneNode> mThermiteSmall;
	shared_ptr<ISceneNode> mThermiteBig;
	shared_ptr<ISceneNode> mMeshLock;
	shared_ptr<ISceneNode> mThermitePlace;
	shared_ptr<ISceneNode> mMeshToSewers;
	shared_ptr<ISceneNode> mEnemySpawnPosition;
	shared_ptr<ISceneNode> mRadioHurtZone;
	unique_ptr<ItemPlace> mThermiteItemPlace;

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
	void OnPlayerEnterDisableSteamZone()
	{
		if(!mStages["EnterDisableSteamZone"]) {
			mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveDisableSteam"));
			mStages["EnterDisableSteamZone"] = true;
		}
	}
public:
	explicit LevelResearchFacility(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer);
	virtual ~LevelResearchFacility();
	virtual void DoScenario() final;
	virtual void Hide() final;
	virtual void Show() final;
	virtual void OnSerialize(SaveFile & out) final;
};


