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
	void Proxy_ReadNote1()
	{
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note1Desc"), mLocalization.GetString("note1"));
	}
	void Proxy_ReadNote2()
	{
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note2Desc"), mLocalization.GetString("note2"));
	}
	void Proxy_ReadNote3()
	{
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note3Desc"), mLocalization.GetString("note3"));
	}
	void Proxy_ReadNote4()
	{
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note4Desc"), mLocalization.GetString("note4"));
	}
	void Proxy_ReadNote5()
	{
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note5Desc"), mLocalization.GetString("note5"));
	}
	void Proxy_ReadNote6()
	{
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note6Desc"), mLocalization.GetString("note6"));
	}
	void Proxy_ReadNote7()
	{
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note7Desc"), mLocalization.GetString("note7"));
	}
	void Proxy_ReadNote8()
	{
		mPlayer->GetInventory()->AddReadedNote(mLocalization.GetString("note8Desc"), mLocalization.GetString("note8"));
	}


	shared_ptr<ISceneNode> mScreamerZone;
	shared_ptr<ISceneNode> mScreamerZone2;
	shared_ptr<ISceneNode> mConcreteWall;
	shared_ptr<ISceneNode> mDeathZone;
	shared_ptr<ISceneNode> mDetonator;
	shared_ptr<ISceneNode> mFindItemsZone;
	shared_ptr<ISceneNode> mNewLevelZone;
	shared_ptr<ISceneNode> mStoneFallZone;
	shared_ptr<ISceneNode> mWireModels[4];
	shared_ptr<ISceneNode> mDetonatorModels[4];
	shared_ptr<ISceneNode> mExplosivesModels[4];
	shared_ptr<ISceneNode> mExplosivesDummy[4];
	shared_ptr<ISceneNode> mExplosionFlashPosition;
	shared_ptr<IPointLight> mExplosionFlashLight;
	shared_ptr<IParticleSystem > mExplosionDustParticleSystem;
	shared_ptr<ISceneNode> mLiftButton;
	shared_ptr<ISceneNode> mExplodedWall;

	shared_ptr<ItemPlace> mDetonatorPlace[4];

	shared_ptr<ISound> mAlertSound;
	shared_ptr<ISound> mExplosionSound;

	shared_ptr<ITimer> mExplosionTimer;
	shared_ptr<ITimer> mBeepSoundTimer;

	bool mDetonatorActivated;

	int mReadyExplosivesCount;
	shared_ptr<IFog> mFog;
	float mBeepSoundTiming;

	unique_ptr<LightAnimator> mExplosionFlashAnimator;
public:
	explicit LevelMine(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer> & playerTransfer);
	virtual ~LevelMine();
	virtual void DoScenario() final;
	virtual void Show() final;
	virtual void Hide() final;
	virtual void OnSerialize(SaveFile & out) final;
};