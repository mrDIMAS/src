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
public:
    // Sounds
    shared_ptr<ruSound> mWindSound;
	shared_ptr<ruSound> mPowerDownSound;
	shared_ptr<ruSound> mMetalWhineSound;
	shared_ptr<ruSound> mMetalStressSound;
	shared_ptr<ruSound> mLiftFallSound;

    // Zones
	shared_ptr<ruSceneNode> mLiftStopZone;
	shared_ptr<ruSceneNode> mTutorialZone1;
	shared_ptr<ruSceneNode> mTutorialZone2;
	shared_ptr<ruSceneNode> mTutorialZone3;
	shared_ptr<ruSceneNode> mTutorialZone4;
	shared_ptr<ruSceneNode> mTutorialZone5;
	shared_ptr<ruSceneNode> mTutorialZone6;

    // Entities
	shared_ptr<ruSceneNode> mHalt;

	shared_ptr<ruLight>mLiftLamp;

	ActionSeries mLiftCrashSeries;

	shared_ptr<Lift> mLift;

    LevelArrival(const unique_ptr<PlayerTransfer> & playerTransfer);
    ~LevelArrival( );

    virtual void DoScenario();
    virtual void Show();
    virtual void Hide();
    virtual void OnSerialize( SaveFile & out ) final;
    virtual void OnDeserialize( SaveFile & in ) final;
};