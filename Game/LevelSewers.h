#pragma once

#include "Level.h"
#include "LightAnimator.h"
#include "AmbientSoundSet.h"
#include "Gate.h"

class LevelSewers : public Level {
private:
	unique_ptr<Keypad> mKeypad1;
	shared_ptr<Door> mDoorToControl;

	unique_ptr<Keypad> mKeypad2;
	shared_ptr<Door> mDoorToCode;

	unique_ptr<Gate> mGate1;
	unique_ptr<Gate> mGate2;
	unique_ptr<Gate> mGateToLift;

	shared_ptr<ruSceneNode> mZoneKnocks;
	shared_ptr<ruSound> mKnocksSound;
	shared_ptr<ruSceneNode> mWater;

	shared_ptr<Zone> mZoneEnemySpawn;

	vector<shared_ptr<ruSceneNode>> mVerticalWaterList;
	float mVerticalWaterFlow;
	shared_ptr<ruSceneNode> mEnemySpawnPosition;
	float mWaterFlow;

	void OnPlayerEnterSpawnEnemyZone() 	{
		if( !mStages[ "EnemySpawned" ] ) {
			CreateEnemy();
			mStages[ "EnemySpawned" ] = true;			
		}
	}
public:
	LevelSewers( );
	~LevelSewers( );

	virtual void DoScenario();
	virtual void Show();
	virtual void Hide();

	virtual void OnSerialize( SaveFile & out ) final;
	virtual void OnDeserialize( SaveFile & in ) final;

	void CreateEnemy();
};