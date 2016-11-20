#pragma once

#include "Level.h"
#include "LightAnimator.h"
#include "AmbientSoundSet.h"
#include "Gate.h"

class LevelSewers : public Level {
private:
	shared_ptr<Keypad> mKeypad1;
	shared_ptr<Door> mDoorToControl;

	shared_ptr<Keypad> mKeypad2;
	shared_ptr<Door> mDoorToCode;

	shared_ptr<Gate> mGate1;
	shared_ptr<Gate> mGate2;
	shared_ptr<Gate> mGateToLift;

	shared_ptr<ruSceneNode> mZoneKnocks;
	shared_ptr<ruSound> mKnocksSound;
	shared_ptr<ruSceneNode> mWater;

	shared_ptr<ruSceneNode> mZoneNextLevel;
	shared_ptr<ruSceneNode> mPumpSwitch;
	shared_ptr<Lift> mLift1;

	shared_ptr<ruLight> mPumpLight[3];
	shared_ptr<ruLight> mPassLightGreen;
	shared_ptr<ruLight> mPassLightRed;

	shared_ptr<ruSound> mWaterPumpSound[3];

	ruAnimation mPumpSwitchAnimation;
	int mDrainTimer;

	vector<shared_ptr<ruSceneNode>> mVerticalWaterList;
	float mVerticalWaterFlow;
	shared_ptr<ruSceneNode> mEnemySpawnPosition;
	float mWaterFlow;

	void OnPlayerEnterSpawnEnemyZone()
	{
		if(!mStages["EnemySpawned"]) {
			CreateEnemy();
			mStages["EnemySpawned"] = true;
		}
	}

	void OnPlayerEnterDrainWaterLevelZone()
	{
		mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveDropWaterLevel"));
	}


public:
	LevelSewers(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer);
	~LevelSewers();

	virtual void DoScenario();
	virtual void Show();
	virtual void Hide();

	virtual void OnSerialize(SaveFile & out) final;

	void CreateEnemy();
};
