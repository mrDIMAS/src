#include "Precompiled.h"
#include "UsableObject.h"
#include "Player.h"
#include "BareHands.h"
#include "Level.h"

UsableObject::UsableObject() : mToNext( false ), mToPrev( false ), mAppear( false ), mPrev( nullptr ), mNext( nullptr ) {

}

UsableObject::~UsableObject() {

}

shared_ptr<ruSceneNode> UsableObject::GetModel() {
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
	// this method is a dirty hack!
	if( mToNext || mToPrev ) {
		mModel->Hide();
		auto & player = Level::Current()->GetPlayer();
		if( mToPrev ) {
			player->mCurrentUsableObject = mPrev;
		} else {
			player->mCurrentUsableObject = mNext;
		}
		player->mCurrentUsableObject->Appear();
		mToNext = false;
		mToPrev = false;
	}
}

UsableObject * UsableObject::Deserialize( SaveFile & s ) {
	UsableObject * uo = nullptr;
	string typeName;
	s & typeName;
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
		uo->OnSerialize(s);
	}
	return uo;
}

void UsableObject::Serialize( SaveFile & s ) {
	string name = typeid(*this).name();
	s & name;
	OnSerialize( s );
}
