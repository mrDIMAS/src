#pragma once

#include "LightAnimator.h"
#include "Level.h"
#include "Enemy.h"
#include "ItemPlace.h"
#include "AmbientSoundSet.h"

class LevelMine : public Level
{
private:
    void UpdateExplodeSequence();
    void CleanUpExplodeArea();
    void CreateItems();

    ruNodeHandle mScreamerZone;
    ruNodeHandle mScreamerZone2;
    ruNodeHandle mConcreteWall;
    ruNodeHandle mDeathZone;
    ruNodeHandle mDetonator;
    ruNodeHandle mFindItemsZone;
    ruNodeHandle mNewLevelZone;
    ruNodeHandle mStoneFallZone;
    ruNodeHandle mWireModels[4];
    ruNodeHandle mDetonatorModels[4];
    ruNodeHandle mExplosivesModels[4];
	ruNodeHandle mExplosivesDummy[4];
	ruNodeHandle mRock[3];
	ruNodeHandle mExplosionFlashPosition;
	ruNodeHandle mExplosionFlashLight;
	ruNodeHandle mExplosionDustParticleSystem;
	ruVector3 mRockPosition[3];

    Item * mExplosives[4];
    Item * mDetonators[4];
    Item * mWires[4];
    Item * mFuel[2];

    ItemPlace * mDetonatorPlace[4];

    ruSoundHandle mAlertSound;
    ruSoundHandle mExplosionSound;

    ruTimerHandle mExplosionTimer;
    ruTimerHandle mBeepSoundTimer;

    bool mDetonatorActivated;

    int mReadyExplosivesCount;

    float mBeepSoundTiming;

    LightAnimator * mExplosionFlashAnimator;

    Enemy * mEnemy;
public:
    explicit LevelMine( );
    virtual ~LevelMine();
    virtual void DoScenario() final;
    virtual void Show() final;
    virtual void Hide() final;
    virtual void OnSerialize( TextFileStream & out ) final;
    virtual void OnDeserialize( TextFileStream & in ) final;
};