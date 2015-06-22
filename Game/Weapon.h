#pragma once

#include "Game.h"
#include "TextFileStream.h"

class Projectile {
private:
	ruNodeHandle mModel;
	ruVector3 mSpeedVector;
	int mLifeTime;
public:
	explicit Projectile( ruNodeHandle model, ruVector3 direction ) {
		mModel = ruCreateNodeInstance( model );
		mSpeedVector = direction.Normalize() * 0.2f;
		mLifeTime = 200;
		ruFreeze( mModel );
	}

	void Update() {
		ruMoveNode( mModel, mSpeedVector );

		if( ruGetContactCount( mModel ) > 0 ) {
			mLifeTime = 0;
		}
		if( mLifeTime <= 0 ) {
			ruFreeSceneNode( mModel );
		}

		mLifeTime--;
	}
};

class Weapon {
public:
	enum class Type {
		Pistol
	};
protected:
	ruNodeHandle mModel;
	ruNodeHandle mShootPoint;
	ruNodeHandle mProjectileModel;
	ruSoundHandle mShotSound;
	vector<Projectile*> mProjectileList;
	int mShotInterval;
	int mProjectileCount;
	Type mType;
	ruNodeHandle mShotFlash;
	ruSoundHandle mEmptySound;
	ruVector3 mShotOffsetTo;
	ruVector3 mShotOffset;
	ruVector3 mOffset;
	ruVector3 mInitialPosition;
	float mShotFlashIntensity;
	float mShakeCoeff;
	virtual void OnShoot();
	bool mVisible;
public:
	bool IsVisible();
	ruNodeHandle GetModel();
	explicit Weapon( ruNodeHandle owner );
	void SetVisible( bool state );
	Type GetType();
	void Shoot();
	void Update();
	bool LoadBullet( );
	void Serialize( TextFileStream & out );
	void Deserialize( TextFileStream & in );
	void SetProjectileCount( int projCount );
	int GetProjectileCount() {
		return mProjectileCount;
	}
};