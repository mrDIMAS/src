#include "Precompiled.h"
#include "Level.h"
#include "InteractiveObject.h"
#include "Player.h"

void InteractiveObject::UpdateFlashing()
{
	if(mFlashDirection) {
		if(mFlashAlbedo < 1) {
			mFlashAlbedo += 0.015f;
		} else {
			mFlashDirection = false;
		}
	} else {
		if(mFlashAlbedo > 0.2) {
			mFlashAlbedo -= 0.015f;
		} else {
			mFlashDirection = true;
		}
	}

	if(mObject) {
		mObject->SetAlbedo(mFlashAlbedo);
	}
}

InteractiveObject::InteractiveObject(shared_ptr<ruSceneNode> object) : mInteractCountLeft(1), mFlashAlbedo(0.2f), mFlashAlbedoTo(1.0f), mFlashSpeed(0.075f), mFlashDirection(true), mObject(object)
{
	mObject->Freeze();
}

InteractiveObject::InteractiveObject() :
	mInteractCountLeft(1),
	mFlashAlbedo(0.0f),
	mFlashAlbedoTo(0.0f),
	mFlashSpeed(0.0f),
	mFlashDirection(false)
{
}

InteractiveObject::~InteractiveObject()
{

}

void InteractiveObject::Update()
{
	if(mInteractCountLeft > 0) {
		auto & player = Game::Instance()->GetLevel()->GetPlayer();
		if(player->mNearestPickedNode == mObject) {
			if(player->IsUseButtonHit()) {

				player->mPickUpAnimation.SetEnabled(true);
				player->SetBodyAnimation(&player->mPickUpAnimation);

				mInteractCountLeft--;
				OnInteract();
				OnInteract.Clear();
			}
		}
	} else {
		mObject->Hide();
		mObject->SetPosition(ruVector3(-666.666, -666.666, -666.666));
	}

	UpdateFlashing();
}

std::string InteractiveObject::GetPickDescription() const
{
	return mPickDesc;
}

void InteractiveObject::SetPickDescription(const string & pd)
{
	mPickDesc = pd;
}
