#pragma once

#include "Level.h"


class LevelArrival : public Level {
private:
	shared_ptr<ItemPlace> mCanisterPlace;
	shared_ptr<ItemPlace> mFusesPlaces[6];
	shared_ptr<ISceneNode> mFusesModels[6];
	shared_ptr<ISceneNode> mPowerSwitch;
	shared_ptr<ISound> mWindSound;
	shared_ptr<Door> mDoor;
	Animation mHydraulicsAnimation;
	Animation mPowerSwitchAnimation;
	Animation mCanisterAnimation;
	Animation mCrowbarAnimation;
	Animation mLiftFallAnimation;
	shared_ptr<Button> mHydraButton;
	shared_ptr<Button> mPumpStartButton;
	shared_ptr<ISceneNode> mLights[2];
	shared_ptr<ISceneNode> mCanisterModel;
	shared_ptr<ISceneNode> mCrowbarModel;
	shared_ptr<ISceneNode> mRustedDoor;
	shared_ptr<ISceneNode> mZoneNewLevelLoad;
	shared_ptr<ISceneNode> mLift;
	shared_ptr<IPointLight> mSparks[2];
	shared_ptr<IParticleSystem> mSparksPS[2];
	shared_ptr<ISound> mElectricMotorSound;
	shared_ptr<ISound> mInsertSound;
	shared_ptr<ISound> mSwitchSound;
	shared_ptr<ISound> mOilSound;
	shared_ptr<ISound> mGatesSound;
	shared_ptr<ISound> mRustedDoorSound;
	shared_ptr<ISound> mLiftSound;
	shared_ptr<ISound> mScratchSound;
	shared_ptr<ISound> mFallSound;
public:
	LevelArrival(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer);
	~LevelArrival();
	virtual void DoScenario();
	virtual void Show();
	virtual void Hide();
	virtual void OnSerialize(SaveFile & out) final;
};