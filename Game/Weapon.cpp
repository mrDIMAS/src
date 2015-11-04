#include "Precompiled.h"

#include "Weapon.h"
#include "SaveFile.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"

Weapon::Weapon() {
	mModel = ruLoadScene( "data/models/hands_pistol/hands_pistol.scene" );
	mShootPoint = ruFindInObjectByName( mModel, "ShootPoint" );
	mShotSound = ruSound::Load3D( "data/sounds/shot3.ogg" );
	mShotSound.Attach( mShootPoint );
	mEmptySound = ruSound::Load3D( "data/sounds/pistol_empty.ogg" );
	mEmptySound.Attach( mShootPoint );
	mShotInterval = 0;
	mProjectileCount = 6;
	mShotFlash = ruCreateLight( LT_POINT );
	mShotFlash.Attach( mModel );
	mShotFlashIntensity = 0.01f;
	mInitialPosition = mModel.GetLocalPosition();
	mShakeCoeff = 0.0f;

	mHideAnim = ruAnimation( 0, 6, 0.7, false );
	mHideAnim.AddFrameListener( 6, ruDelegate::Bind( this, &Weapon::Proxy_Hide ));
	mShowAnim = ruAnimation( 6, 12, 0.7, false );
	mIdleAnim = ruAnimation( 18, 24, 4.0, true );
}

bool Weapon::LoadBullet() {
	if( mProjectileCount < 8 ) {
		mProjectileCount++;
		return true;
	}

	return false;
}


bool Weapon::IsVisible() {
	return mVisible;
}

void Weapon::SetProjectileCount( int projCount ) {
	mProjectileCount = projCount;
}

void Weapon::OnDeserialize( SaveFile & in ) {
	mProjectileCount = in.ReadInteger();
	mShotInterval = in.ReadInteger();
	mShotFlashIntensity = in.ReadFloat();
}

void Weapon::OnSerialize( SaveFile & out ) {
	out.WriteInteger( mProjectileCount );
	out.WriteInteger( mShotInterval );
	out.WriteFloat( mShotFlashIntensity );
}

void Weapon::Update() {
	if( mAppear ) {
		mShowAnim.Rewind();
		mModel.SetAnimation( &mShowAnim );
		mShowAnim.enabled = true;
		mAppear = false;
	}

	if( mToPrev || mToNext ) {
		if( !mHideAnim.enabled ) {
			mHideAnim.Rewind();
		}
		mModel.SetAnimation( &mHideAnim );
		mHideAnim.enabled = true;		
	}

	mHideAnim.Update();
	mShowAnim.Update();
	mIdleAnim.Update();

	if( ruIsMouseHit( MB_Left ) && !pPlayer->mNodeInHands.IsValid() ) {
		if( mProjectileCount > 0 ) {
			if( mShotInterval <= 0 ) {			
				mShotInterval = 35;
				mShotFlashIntensity = 5.0f;
				mProjectileCount--;
				mShotSound.Play();
				mShotOffsetTo = ruVector3( 0, 0.065, -0.18 );

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
		} else {
			mEmptySound.Play();
		}
	}

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


	if( !mShowAnim.enabled && !mHideAnim.enabled ) {
		mModel.SetAnimation( &mIdleAnim );
		mIdleAnim.enabled = true;
	}
}

int Weapon::GetProjectileCount() {
	return mProjectileCount;
}

Item* Weapon::CreateItem() {
	return new Item( Item::Type::Pistol );
}

void Weapon::Proxy_Hide()
{
	SwitchIfAble();
}
