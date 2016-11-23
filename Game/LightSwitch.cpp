#include "Precompiled.h"
#include "LightSwitch.h"
#include "Level.h"

LightSwitch::LightSwitch(const shared_ptr<ISceneNode>& model, const vector<shared_ptr<ILight>>& lights, bool enabled) : mModel(model), mLights(lights), mEnabled(enabled) {
	mSwitchOnAnim = Animation(0, model->GetTotalAnimationFrameCount() / 2, 0.5f, false);
	mSwitchOffAnim = Animation(model->GetTotalAnimationFrameCount() / 2, model->GetTotalAnimationFrameCount() - 1, 0.5f, false);
	mSwitchSound = model->GetFactory()->GetEngineInterface()->GetSoundSystem()->LoadSound3D("data/sounds/lever.ogg");
	mSwitchSound->Attach(model);
}

void LightSwitch::AddLight(const shared_ptr<ILight>& light) {
	mLights.push_back(light);
}

void LightSwitch::Update() {
	mSwitchOffAnim.Update();
	mSwitchOnAnim.Update();
	auto & player = Game::Instance()->GetLevel()->GetPlayer();
	if(player->mNearestPickedNode == mModel) {
		player->GetHUD()->SetAction(player->mKeyUse, player->GetLocalization()->GetString("lightSwitch"));
		if(player->IsUseButtonHit()) {
			if(mEnabled) {
				mSwitchOffAnim.Rewind();
				mSwitchOffAnim.SetEnabled(true);
				mModel->SetAnimation(&mSwitchOffAnim);
				mSwitchSound->Play();
			} else {
				mSwitchOnAnim.Rewind();
				mSwitchOnAnim.SetEnabled(true);
				mModel->SetAnimation(&mSwitchOnAnim);
				mSwitchSound->Play();
			}
			mEnabled = !mEnabled;
		}
	}
	for(auto light : mLights) {
		if(mEnabled) {
			light->Show();
		} else {
			light->Hide();
		}
	}
}

void LightSwitch::Serialize(SaveFile & s) {
	s & mEnabled;
}
