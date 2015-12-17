#include "Precompiled.h"
#include "Syringe.h"
#include "Player.h"
#include "Item.h"

Syringe::Syringe( ) : mDozeCount( 2 ) {
	mModel = ruSceneNode::LoadFromFile( "data/models/hands_syringe/hands_syringe.scene" );
	mModel->SetDepthHack( 0.155f );

	mUseAnim = ruAnimation( 6, 18, 2.5, false );
	mUseAnim.AddFrameListener( 12, ruDelegate::Bind( this, &Syringe::Proxy_HealPlayer ));
	mUseAnim.AddFrameListener( 18, ruDelegate::Bind( this, &Syringe::Proxy_Use ));

	mShowAnim = ruAnimation( 0, 6, 0.7, false );

	mShowDepletedAnim = ruAnimation( 22, 27, 0.7, false );

	mHideAnim = ruAnimation( 38, 45, 0.7, false );
	mHideAnim.AddFrameListener( 45, ruDelegate::Bind( this, &Syringe::Proxy_Hide ));

	mHideDepletedAnim = ruAnimation( 18, 22, 0.7, false );
	mHideDepletedAnim.AddFrameListener( 22, ruDelegate::Bind( this, &Syringe::Proxy_Hide ));

	mIdleAnim = ruAnimation( 28, 38, 3.0f, true );
	mModel->SetAnimation( &mIdleAnim );

	mIdleDepletedAnim = ruAnimation( 46, 55, 3.0f, true );
}

void Syringe::Proxy_Hide() {
	SwitchIfAble();
}

void Syringe::Proxy_HealPlayer() {
	pPlayer->Heal( 80 );
}

void Syringe::Update() {
	if( mAppear ) {
		if( mDozeCount > 0 ) {
			mShowAnim.Rewind();
			mModel->SetAnimation( &mShowAnim );
			mShowAnim.enabled = true;				
		} else {
			mShowDepletedAnim.Rewind();
			mModel->SetAnimation( &mShowDepletedAnim );
			mShowDepletedAnim.enabled = true;
		}
		mAppear = false;
	}

	if( (mToNext || mToPrev) ) {
		if( !mUseAnim.enabled && !mShowAnim.enabled && !mShowDepletedAnim.enabled && !mHideDepletedAnim.enabled && !mHideAnim.enabled ) {
			if( mDozeCount > 0 ) {
				mModel->SetAnimation( &mHideAnim );
				mHideAnim.Rewind();
				mHideAnim.enabled = true;
			} else {				
				if( !mHideDepletedAnim.enabled ) {
					mModel->SetAnimation( &mHideDepletedAnim );
					mHideDepletedAnim.Rewind();
					mHideDepletedAnim.enabled = true;					
				}
			}
		}
	}

	mUseAnim.Update();
	mIdleAnim.Update();
	mShowAnim.Update();
	mHideAnim.Update();
	mShowDepletedAnim.Update();
	mHideDepletedAnim.Update();
	mIdleDepletedAnim.Update();

	if( ruIsMouseHit( MB_Left ) && !mShowAnim.enabled && !mHideAnim.enabled && !mUseAnim.enabled && !mHideDepletedAnim.enabled && !mShowDepletedAnim.enabled ) {
		if( mDozeCount > 0 ) {
			mUseAnim.Rewind();
			mModel->SetAnimation( &mUseAnim );
			mUseAnim.enabled = true;
			mIdleAnim.enabled = false;
			mDozeCount--;
		}
	}

	if( !mShowAnim.enabled && !mShowDepletedAnim.enabled && !mUseAnim.enabled && !mHideAnim.enabled && !mHideDepletedAnim.enabled ) {
		if( mDozeCount > 0 ) {
			mModel->SetAnimation( &mIdleAnim );
			mIdleAnim.enabled = true;
		} else {
			mModel->SetAnimation( &mIdleDepletedAnim );
			mIdleDepletedAnim.enabled = true;			
		}
	}
}

void Syringe::OnDeserialize( SaveFile & in ) {
	mDozeCount = in.ReadInteger();
}

void Syringe::OnSerialize( SaveFile & out ) {
	out.WriteInteger( mDozeCount );
}

void Syringe::AddDoze() {
	mDozeCount++;
}

void Syringe::Proxy_Use() {
	if( mNext ) {
		Next();
	} else if ( mPrev ) {
		Prev();
	}
	if( !mHideDepletedAnim.enabled ) {
		mModel->SetAnimation( &mHideDepletedAnim );
		mHideDepletedAnim.Rewind();
		mHideDepletedAnim.enabled = true;					
	}
}
