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
	shared_ptr<ruSceneNode> mLiftStopZone;
	shared_ptr<ruParticleSystem > mGeneratorSmoke;
	ruVector3 mGeneratorSmokePosition;

    // Entities
    shared_ptr<ruSceneNode> mGenerator;

    shared_ptr<ruSceneNode> mLamp1;
    shared_ptr<ruSceneNode> mLamp2;

	shared_ptr<ruLight>mLiftLamp;

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