#pragma once

#include "LightAnimator.h"
#include "Level.h"
#include "Enemy.h"
#include "ItemPlace.h"
#include "AmbientSoundSet.h"

class LevelMine : public Level {
private:
    void UpdateExplodeSequence();
    void CleanUpExplodeArea();
    void CreateItems();

	// COPY-PASTE! ARGH!
	void Proxy_ReadNote1() {
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note1Desc"), mLocalization.GetString("note1"));
	}
	void Proxy_ReadNote2() {
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note2Desc"), mLocalization.GetString("note2"));
	}
	void Proxy_ReadNote3() {
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note3Desc"), mLocalization.GetString("note3"));
	}
	void Proxy_ReadNote4() {
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note4Desc"), mLocalization.GetString("note4"));
	}
	void Proxy_ReadNote5() {
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note5Desc"), mLocalization.GetString("note5"));
	}
	void Proxy_ReadNote6() {
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note6Desc"), mLocalization.GetString("note6"));
	}
	void Proxy_ReadNote7() {
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note7Desc"), mLocalization.GetString("note7"));
	}
	void Proxy_ReadNote8() {
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note8Desc"), mLocalization.GetString("note8"));
	}


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
    shared_ptr<ruSceneNode> mExplosionFlashPosition;
    shared_ptr<ruPointLight> mExplosionFlashLight;
    shared_ptr<ruParticleSystem > mExplosionDustParticleSystem;
	shared_ptr<ruSceneNode> mLiftButton;
	shared_ptr<ruSceneNode> mExplodedWall;

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
    explicit LevelMine(const unique_ptr<PlayerTransfer> & playerTransfer);
    virtual ~LevelMine();
    virtual void DoScenario() final;
    virtual void Show() final;
    virtual void Hide() final;
    virtual void OnSerialize( SaveFile & out ) final;
};