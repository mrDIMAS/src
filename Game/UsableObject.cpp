#include "Precompiled.h"
#include "UsableObject.h"
#include "Player.h"
#include "BareHands.h"


UsableObject::UsableObject() : mToNext( false ), mToPrev( false ), mAppear( false ), mPrev( nullptr ), mNext( nullptr ) {

}

UsableObject::~UsableObject() {
	mModel->Free();
}

ruSceneNode * UsableObject::GetModel() {
	return mModel;
}

void UsableObject::Prev() {
	if( mPrev ) {
		mToNext = false;
		mToPrev = true;
	}
}

void UsableObject::Next() {
	if( mNext ) {
		mToNext = true;
		mToPrev = false;
	}
}

void UsableObject::Link( UsableObject * other ) {
	mNext = other;
	other->mPrev = this;
}

void UsableObject::Appear() {
	mAppear = true;
}

void UsableObject::SwitchIfAble() {
	if( mToNext || mToPrev ) {
		mModel->Hide();
		if( mToPrev ) {
			pPlayer->mCurrentUsableObject = mPrev;
		} else {
			pPlayer->mCurrentUsableObject = mNext;
		}
		pPlayer->mCurrentUsableObject->Appear();
		mToNext = false;
		mToPrev = false;
	}
}

UsableObject * UsableObject::Deserialize( SaveFile & in ) {
	UsableObject * uo = nullptr;
	string typeName = in.ReadString();
	if( typeName == typeid( Flashlight ).name() ) {
		uo = new Flashlight;			
	} else if( typeName == typeid( Weapon ).name()) {
		uo = new Weapon;
	} else if( typeName == typeid( Syringe ).name()) {
		uo = new Syringe;
	} else if( typeName == typeid( BareHands ).name()) {
		uo = new BareHands;
	}
	if( uo ) {
		uo->OnDeserialize( in );
	}
	return uo;
}

void UsableObject::Serialize( SaveFile & out ) {
	out.WriteString( typeid( *this ).name() );
	OnSerialize( out );
}
