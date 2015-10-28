#include "Precompiled.h"
#include "Syringe.h"
#include "Player.h"
#include "Item.h"

Syringe::Syringe( ) :  mDepleted( false ) {
	mModel = ruLoadScene( "data/models/hands_syringe/hands_syringe.scene" );


	mUseAnim = ruAnimation( 0, 22, 2.5, false );
	mUseAnim.AddFrameListener( 6, ruDelegate::Bind( this, &Syringe::Proxy_HealPlayer ));

	mIdleAnim = ruAnimation( 23, 33, 1.0f, true );
	mModel.SetAnimation( &mIdleAnim );
}

void Syringe::Proxy_HealPlayer() {
	pPlayer->Heal( 80 );
}

void Syringe::Update() {
	if( mAppear ) {
		mModel.Show();
		mAppear = false;
	}

	SwitchIfAble();
	/*
	if( mToNext || mToPrev ) {
		mModel.Hide();
		if( mToPrev ) {
			pPlayer->mCurrentUsableObject = mPrev;
			mToPrev = false;
		} else {
			pPlayer->mCurrentUsableObject = mNext;
			mToNext = false;
		}
		pPlayer->mCurrentUsableObject->Appear();		
	}*/

	mUseAnim.Update();
	mIdleAnim.Update();
	if( ruIsMouseHit( MB_Left )) {
		if( !mDepleted ) {
			mModel.SetAnimation( &mUseAnim );
			mUseAnim.enabled = true;
			mIdleAnim.enabled = false;
			mDepleted = true;
		}
	}
}

void Syringe::OnDeserialize( SaveFile & in )
{

}

void Syringe::OnSerialize( SaveFile & out ) {
	
}

Item* Syringe::CreateItem()
{
	return new Item( mModel, Item::Type::Syringe );
}
