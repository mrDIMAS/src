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
	void GeneratorEnableAction( );
	void ActGenerator_Start();
	void ActGenerator_OnLine();
	bool mChangeLevel;
public:
    // Sounds
    ruSound mWindSound;
    ruSound mGeneratorSound;
	ruSound mPowerDownSound;
	ruSound mMetalWhineSound;
	ruSound mMetalStressSound;
	ruSound mLiftFallSound;
	ruSound mGeneratorStartSound;
    // Zones
	ruSceneNode mLiftStopZone;
	ruSceneNode mGeneratorSmoke;
	ruVector3 mGeneratorSmokePosition;

    // Entities
    ruSceneNode mGenerator;

    ruSceneNode mLamp1;
    ruSceneNode mLamp2;

	ruLight mLiftLamp;

	ActionSeries mLiftCrashSeries;
	ActionSeries mGeneratorStartSeries;

	shared_ptr<Lift> mLift;
	Button * mGeneratorButton;

	bool mPowerRestored;

    LevelArrival( );
    ~LevelArrival( );

    virtual void DoScenario();
    virtual void Show();
    virtual void Hide();
    virtual void OnSerialize( SaveFile & out ) final;
    virtual void OnDeserialize( SaveFile & in ) final;
};