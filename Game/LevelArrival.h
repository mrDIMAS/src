#pragma once

#include "Level.h"
#include "LightAnimator.h"
#include "AmbientSoundSet.h"

class LevelArrival : public Level {
private:
	void ActLiftCrash_PowerDown();
	void ActLiftCrash_AfterPowerDown();
	void ActLiftCrash_AfterFirstStressSound();
	void ActLiftCrash_AfterFalldown();
	bool mChangeLevel;
	shared_ptr<IFog> mFog;
	shared_ptr<ISound> mWoodHitSound;
public:
	// Sounds
	shared_ptr<ISound> mWindSound;
	shared_ptr<ISound> mPowerDownSound;
	shared_ptr<ISound> mMetalWhineSound;
	shared_ptr<ISound> mMetalStressSound;
	shared_ptr<ISound> mLiftFallSound;

	// Zones
	shared_ptr<ISceneNode> mLiftStopZone;
	shared_ptr<ISceneNode> mTutorialZone1;
	shared_ptr<ISceneNode> mTutorialZone2;
	shared_ptr<ISceneNode> mTutorialZone3;
	shared_ptr<ISceneNode> mTutorialZone4;
	shared_ptr<ISceneNode> mTutorialZone5;
	shared_ptr<ISceneNode> mTutorialZone6;
	shared_ptr<ISceneNode> mTutorialZone7;
	shared_ptr<ISceneNode> mTutorialZone8;

	// Entities
	shared_ptr<ISceneNode> mHalt;

	shared_ptr<ILight>mLiftLamp;

	ActionSeries mLiftCrashSeries;

	shared_ptr<Lift> mLift;

	LevelArrival(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer);
	~LevelArrival();

	virtual void DoScenario();
	virtual void Show();
	virtual void Hide();
	virtual void OnSerialize(SaveFile & out) final;
};