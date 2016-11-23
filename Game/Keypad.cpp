#include "Precompiled.h"
#include "Level.h"
#include "Keypad.h"

void Keypad::Update()
{
	auto & player = Game::Instance()->GetLevel()->GetPlayer();
	for(int i = 0; i < 10; i++) {
		if(player->mNearestPickedNode == mKeys[i]) {
			if(!mKeyState[i]) {
				player->GetHUD()->SetAction(player->mKeyUse, StringBuilder() << player->mLocalization.GetString("pressButton") << " " << i);
				if(player->IsUseButtonHit()) {
					mCurrentCode += to_string(i);
					mKeyState[i] = true;
					mButtonPushSound->Play();
					if(mCurrentCode.size() == 4) {
						if(mCurrentCode == mCodeToUnlock) {
							if(mDoorToUnlock.use_count()) {
								mDoorToUnlock.lock()->SetLocked(false);
								mDoorToUnlock.lock()->Open();
							}
						}
						Reset();
					} else {
						mKeys[i]->SetPosition(mKeysInitialPosition[i] + mKeysPressedOffsets[i]);
					}
				}
			}
		}
	}

	if(player->mNearestPickedNode == mKeyCancel) {
		player->GetHUD()->SetAction(player->mKeyUse, player->mLocalization.GetString("resetButtons"));
		if(player->IsUseButtonHit()) {
			Reset();
		}
	}
}

Keypad::Keypad(shared_ptr<ISceneNode> keypad, shared_ptr<ISceneNode> key0, shared_ptr<ISceneNode> key1, shared_ptr<ISceneNode> key2,
	shared_ptr<ISceneNode> key3, shared_ptr<ISceneNode> key4, shared_ptr<ISceneNode> key5, shared_ptr<ISceneNode> key6,
	shared_ptr<ISceneNode> key7, shared_ptr<ISceneNode> key8, shared_ptr<ISceneNode> key9, shared_ptr<ISceneNode> keyCancel,
	weak_ptr<Door> doorToUnlock, string codeToUnlock)
{
	mKeypad = keypad;
	mKeys[0] = key0;
	mKeys[1] = key1;
	mKeys[2] = key2;
	mKeys[3] = key3;
	mKeys[4] = key4;
	mKeys[5] = key5;
	mKeys[6] = key6;
	mKeys[7] = key7;
	mKeys[8] = key8;
	mKeys[9] = key9;
	mKeyCancel = keyCancel;

	mDoorToUnlock = doorToUnlock;
	mCodeToUnlock = codeToUnlock;

	for(int i = 0; i < 10; i++) {
		mKeysInitialPosition[i] = mKeys[i]->GetPosition();
		Vector3 min = mKeys[i]->GetAABBMin();
		Vector3 max = mKeys[i]->GetAABBMax();
		Vector3 size = (max - min) / 2;
		Vector3 right = mKeys[i]->GetRightVector();
		mKeysPressedOffsets[i] = right * (size / 2);
	}

	if(mDoorToUnlock.use_count()) {
		mDoorToUnlock.lock()->SetLocked(true);
	}

	auto soundSystem = mKeypad->GetFactory()->GetEngineInterface()->GetSoundSystem();

	mButtonPushSound = soundSystem->LoadSound3D("data/sounds/button_push.ogg");
	mButtonPopSound = soundSystem->LoadSound3D("data/sounds/button_pop.ogg");

	mButtonPushSound->SetPosition(mKeypad->GetPosition());
	mButtonPopSound->SetPosition(mKeypad->GetPosition());

	Reset();
}

void Keypad::Reset()
{
	for(int i = 0; i < 10; i++) {
		mKeyState[i] = false;
		mCurrentCode.clear();
		mKeys[i]->SetPosition(mKeysInitialPosition[i]);
	}
	mButtonPopSound->Play();
}

Keypad::~Keypad()
{

}
