#include "Precompiled.h"
#include "BareHands.h"

void BareHands::Update() {
	if( mAppear ) {
		mModel.Show();
		mAppear = false;
	}
	SwitchIfAble();
}

Item* BareHands::CreateItem() {
	return nullptr;
}

BareHands::BareHands() {
	mModel = ruCreateSceneNode();
}

void BareHands::OnDeserialize( SaveFile & in ) {

}

void BareHands::OnSerialize( SaveFile & out ) {

}
