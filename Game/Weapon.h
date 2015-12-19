#pragma once

#include "Game.h"
#include "SaveFile.h"
#include "UsableObject.h"

class Weapon : public UsableObject {
protected:
	ruAnimation mShowAnim;
	ruAnimation mHideAnim;
	ruAnimation mIdleAnim;
	void Proxy_Hide();
	shared_ptr<ruSceneNode> mShootPoint;
	shared_ptr<ruSceneNode> mProjectileModel;
	ruSound mShotSound;
	int mShotInterval;
	int mProjectileCount;
	shared_ptr<ruPointLight>mShotFlash;
	ruSound mEmptySound;
	ruVector3 mShotOffsetTo;
	ruVector3 mShotOffset;
	ruVector3 mOffset;
	ruVector3 mInitialPosition;
	float mShotFlashIntensity;
	float mShakeCoeff;
	bool mVisible;
	virtual void OnSerialize( SaveFile & out ) final;
	virtual void OnDeserialize( SaveFile & in ) final;
public:
	bool IsVisible();
	explicit Weapon();
	~Weapon() {

	}
	bool LoadBullet( );
	void SetProjectileCount( int projCount );
	int GetProjectileCount();
	virtual void Update() final;
	virtual Item::Type GetItemType( ) final {
		return Item::Type::Pistol;
	}
};