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

    shared_ptr<ruSceneNode> mScreamerZone;
    shared_ptr<ruSceneNode> mScreamerZone2;
    shared_ptr<ruSceneNode> mConcreteWall;
    shared_ptr<ruSceneNode> mDeathZone;
    shared_ptr<ruSceneNode> mDetonator;
    shared_ptr<ruSceneNode> mFindItemsZone;
    shared_ptr<ruSceneNode> mNewLevelZone;
    shared_ptr<ruSceneNode> mStoneFallZone;
    shared_ptr<ruSceneNode> mWireModels[4];
    shared_ptr<ruSceneNode> mDetonatorModels[4];
    shared_ptr<ruSceneNode> mExplosivesModels[4];
    shared_ptr<ruSceneNode> mExplosivesDummy[4];
    shared_ptr<ruSceneNode> mRock[3];
    shared_ptr<ruSceneNode> mExplosionFlashPosition;
    shared_ptr<ruPointLight>mExplosionFlashLight;
    shared_ptr<ruParticleSystem > mExplosionDustParticleSystem;
	shared_ptr<ruSceneNode> mLiftButton;

    ruVector3 mRockPosition[3];

    shared_ptr<ItemPlace> mDetonatorPlace[4];

    shared_ptr<ruSound> mAlertSound;
    shared_ptr<ruSound> mExplosionSound;

    shared_ptr<ruTimer> mExplosionTimer;
    shared_ptr<ruTimer> mBeepSoundTimer;

    bool mDetonatorActivated;

    int mReadyExplosivesCount;

    float mBeepSoundTiming;

    unique_ptr<LightAnimator> mExplosionFlashAnimator;

    
public:
    explicit LevelMine( );
    virtual ~LevelMine();
    virtual void DoScenario() final;
    virtual void Show() final;
    virtual void Hide() final;
    virtual void OnSerialize( SaveFile & out ) final;
    virtual void OnDeserialize( SaveFile & in ) final;
};