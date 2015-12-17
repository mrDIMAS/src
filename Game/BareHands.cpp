#include "Precompiled.h"
#include "BareHands.h"

void BareHands::Update() {
	if( mAppear ) {
		mModel->Show();
		mAppear = false;
	}
	SwitchIfAble();
}

BareHands::BareHands() {
	mModel = ruSceneNode::Create();
}

void BareHands::OnDeserialize( SaveFile & in ) {

}

void BareHands::OnSerialize( SaveFile & out ) {

}
