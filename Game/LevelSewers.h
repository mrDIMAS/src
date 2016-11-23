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

	shared_ptr<ISceneNode> mZoneKnocks;
	shared_ptr<ISound> mKnocksSound;
	shared_ptr<ISceneNode> mWater;

	shared_ptr<ISceneNode> mZoneNextLevel;
	shared_ptr<ISceneNode> mPumpSwitch;
	shared_ptr<Lift> mLift1;

	shared_ptr<ILight> mPumpLight[3];
	shared_ptr<ILight> mPassLightGreen;
	shared_ptr<ILight> mPassLightRed;

	shared_ptr<ISound> mWaterPumpSound[3];

	Animation mPumpSwitchAnimation;
	int mDrainTimer;

	vector<shared_ptr<ISceneNode>> mVerticalWaterList;
	float mVerticalWaterFlow;
	shared_ptr<ISceneNode> mEnemySpawnPosition;
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
