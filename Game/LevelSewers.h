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

	shared_ptr<Zone> mZoneEnemySpawn;
	shared_ptr<Zone> mZoneDropWaterLevel;
	shared_ptr<Lift> mLift1;
	shared_ptr<ruLight> mPassLightGreen;
	shared_ptr<ruLight> mPassLightRed;

	vector<shared_ptr<ruSceneNode>> mVerticalWaterList;
	float mVerticalWaterFlow;
	shared_ptr<ruSceneNode> mEnemySpawnPosition;
	float mWaterFlow;

	void OnPlayerEnterSpawnEnemyZone() {
		if (!mStages["EnemySpawned"]) {
			CreateEnemy();
			mStages["EnemySpawned"] = true;
		}
	}

	void OnPlayerEnterDropWaterLevelZone() {
		mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveDropWaterLevel"));
	}
public:
	LevelSewers(const unique_ptr<PlayerTransfer> & playerTransfer);
	~LevelSewers();

	virtual void DoScenario();
	virtual void Show();
	virtual void Hide();

	virtual void OnSerialize(SaveFile & out) final;

	void CreateEnemy();
};
