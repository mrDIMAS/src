#pragma once

#include "LightAnimator.h"
#include "Level.h"
#include "Enemy.h"
#include "ItemPlace.h"
#include "AmbientSoundSet.h"

class LevelMine : public Level {
private:
	void Proxy_GiveExplosives() {
		pPlayer->AddItem( Item::Type::Explosives );
	}
	void Proxy_GiveDetonator( ) {
		pPlayer->AddItem( Item::Type::Detonator );
	}
	void Proxy_GiveWires( ) {
		pPlayer->AddItem( Item::Type::Wires );
	}
	void Proxy_GivePistol( ) {
		pPlayer->AddUsableObject( new Weapon );
	}
	void Proxy_GiveFuel( ) {
		pPlayer->AddItem( Item::Type::FuelCanister );
	}
	void Proxy_GiveSyringe( ) {
		pPlayer->AddUsableObject( new Syringe );
	}
    void UpdateExplodeSequence();
    void CleanUpExplodeArea();
    void CreateItems();

    ruSceneNode mScreamerZone;
    ruSceneNode mScreamerZone2;
    ruSceneNode mConcreteWall;
    ruSceneNode mDeathZone;
    ruSceneNode mDetonator;
    ruSceneNode mFindItemsZone;
    ruSceneNode mNewLevelZone;
    ruSceneNode mStoneFallZone;
    ruSceneNode mWireModels[4];
    ruSceneNode mDetonatorModels[4];
    ruSceneNode mExplosivesModels[4];
    ruSceneNode mExplosivesDummy[4];
    ruSceneNode mRock[3];
    ruSceneNode mExplosionFlashPosition;
    ruLight mExplosionFlashLight;
    ruSceneNode mExplosionDustParticleSystem;
    ruVector3 mRockPosition[3];

    shared_ptr<ItemPlace> mDetonatorPlace[4];

    ruSound mAlertSound;
    ruSound mExplosionSound;

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
    virtual void OnSerialize( SaveFile & out ) final;
    virtual void OnDeserialize( SaveFile & in ) final;
};