#include "Precompiled.h"
#include "Button.h"
#include "Player.h"
#include "Utils.h"
#include "Level.h"

void Button::Update() {
	auto & player = Level::Current()->GetPlayer();
	if (player->mNearestPickedNode == mNode) {
		player->GetHUD()->SetAction(player->mKeyUse, "Запустить генератор"); // WAT?????
		if (ruInput::IsKeyHit(player->mKeyUse)) {
			OnPush.DoActions();
			mPushSound->Play();
			mPush = true;
		}
		if (ruInput::IsKeyDown(player->mKeyUse)) {
			mNode->SetPosition(mInitialPosition - mAxis * mSize / 2.0f);
		}
	}
	if (!ruInput::IsKeyDown(player->mKeyUse)) {
		mNode->SetPosition(mInitialPosition);
	}
	if (mPush) {
		mPopSound->Play();
		mPush = false;
	}
}

Button::Button(shared_ptr<ruSceneNode> node, const ruVector3 & axis, shared_ptr<ruSound> pushSound, shared_ptr<ruSound> popSound)
	: mNode(node), mAxis(axis), mPush(false), mPushSound(pushSound), mPopSound(popSound) {
	mInitialPosition = mNode->GetPosition();
	mSize = (mNode->GetAABBMax() - mNode->GetAABBMin()).Abs();
	mPushSound->Attach(mNode);
	mPopSound->Attach(mNode);
}
