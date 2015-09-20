#pragma once

#include "Game.h"
#include "SaveFile.h"

class Projectile {
private:
	ruSceneNode mModel;
	ruVector3 mSpeedVector;
	int mLifeTime;
public:
	explicit Projectile( ruSceneNode model, ruVector3 direction ) {
		mModel = ruCreateNodeInstance( model );
		mSpeedVector = direction.Normalize() * 0.2f;
		mLifeTime = 200;
		mModel.Freeze();
	}

	void Update() {
		mModel.Move( mSpeedVector );

		if( mModel.GetContactCount() > 0 ) {
			mLifeTime = 0;
		}
		if( mLifeTime <= 0 ) {
			mModel.Free();
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
	ruSceneNode mModel;
	ruSceneNode mShootPoint;
	ruSceneNode mProjectileModel;
	ruSound mShotSound;
	vector<Projectile*> mProjectileList;
	int mShotInterval;
	int mProjectileCount;
	Type mType;
	ruSceneNode mShotFlash;
	ruSound mEmptySound;
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
	ruSceneNode GetModel();
	explicit Weapon( ruSceneNode owner );
	void SetVisible( bool state );
	Type GetType();
	void Shoot();
	void Update();
	bool LoadBullet( );
	void Serialize( SaveFile & out );
	void Deserialize( SaveFile & in );
	void SetProjectileCount( int projCount );
	int GetProjectileCount() {
		return mProjectileCount;
	}
};