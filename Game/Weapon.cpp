#include "Precompiled.h"

#include "Weapon.h"
#include "SaveFile.h"
#include "Player.h"
#include "Enemy.h"

void Weapon::Deserialize( SaveFile & in ) {
	mType = (Type)in.ReadInteger();
	mProjectileCount = in.ReadInteger();
	mShotInterval = in.ReadInteger();
	mShotFlashIntensity = in.ReadFloat();
}

void Weapon::Serialize( SaveFile & out ) {
	out.WriteInteger( 0 );
	out.WriteInteger( mProjectileCount );
	out.WriteInteger( mShotInterval );
	out.WriteFloat( mShotFlashIntensity );
}

void Weapon::Update() {
	mModel.SetLocalPosition( mInitialPosition + mShotOffset + mOffset * 0.25f );
	if( (mShotOffset - mShotOffsetTo).Length2() < 0.025 ) {
		mShotOffsetTo = ruVector3( 0, 0, 0 );
	}
	mShotOffset = mShotOffset.Lerp( mShotOffsetTo, 0.2 );
	ruSetLightRange( mShotFlash, mShotFlashIntensity );
	mShotFlashIntensity -= 1.05f;
	if( mShotFlashIntensity < 0 ) {
		mShotFlashIntensity = 0;
	} 
	mShotInterval--;
	if( pPlayer->mMoved ) {
		if( pPlayer->mRunning ) {
			mShakeCoeff += 11.5f * g_dt;
		} else {
			mShakeCoeff += 7.5f * g_dt;
		}
		mOffset = ruVector3( cosf( mShakeCoeff * 0.5f ) * 0.005f, sinf( mShakeCoeff ) * 0.01f, 0.0f );
	}
}

void Weapon::Shoot() {
	if( mProjectileCount > 0 ) {
		if( mShotInterval <= 0 ) {			
			mShotInterval = 20;
			mShotFlashIntensity = 5.0f;
			mProjectileCount--;
			mShotSound.Play();
			mShotOffsetTo = ruVector3( 0, 0.065, -0.18 );
			OnShoot();			
		}
	} else {
		mEmptySound.Play();
	}
}

Weapon::Type Weapon::GetType() {
	return mType;
}

void Weapon::SetVisible( bool state ) {
	mVisible = state;
	if( state ) {
		mModel.Show();
	} else {
		mModel.Hide();
	}
}

Weapon::Weapon( ruSceneNode owner ) {
	mModel = ruLoadScene( "data/models/hands_pistol/handspistol.scene" );
	mShootPoint = ruFindInObjectByName( mModel, "ShootPoint" );
	mModel.Attach( owner );
	mShotSound = ruSound::Load3D( "data/sounds/shot3.ogg" );
	mShotSound.Attach( mShootPoint );
	mEmptySound = ruSound::Load3D( "data/sounds/pistol_empty.ogg" );
	mEmptySound.Attach( mShootPoint );
	mShotInterval = 0;
	mProjectileCount = 6;
	mType = Type::Pistol;
	mShotFlash = ruCreateLight( LT_POINT );
	mShotFlash.Attach( mModel );
	mShotFlashIntensity = 0.01f;
	mInitialPosition = mModel.GetLocalPosition();
	mShakeCoeff = 0.0f;
}

void Weapon::OnShoot() {
	ruVector3 look = mModel.GetAbsoluteLookVector();
	ruSceneNode node = ruCastRay( mShootPoint.GetPosition() + look * 0.1, mShootPoint.GetPosition() + look * 1000 );
	if( node.IsValid() ) {
		for( auto pEnemy : Enemy::msEnemyList ) {
			if( node == pEnemy->GetBody() ) {
				pEnemy->Damage( 1000 );
			}
		}
		node.AddForce( look.Normalize() * 200 );
	}
}

bool Weapon::LoadBullet() {
	if( mProjectileCount < 8 ) {
		mProjectileCount++;
		return true;
	}

	return false;
}

ruSceneNode Weapon::GetModel() {
	return mModel;
}

bool Weapon::IsVisible() {
	return mVisible;
}

void Weapon::SetProjectileCount( int projCount )
{
	mProjectileCount = projCount;
}
