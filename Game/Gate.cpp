#include "Precompiled.h"
#include "Gate.h"
#include "Level.h"

void Gate::Update() {
	mOpenAnim.Update();
	mCloseAnim.Update();

	for(int i = 0; i < 4; i++) {
		mButtonPushAnim[i].Update();
	}

	auto & player = Game::Instance()->GetLevel()->GetPlayer();

	if(player->mNearestPickedNode == mButtonOpen[0] || player->mNearestPickedNode == mButtonOpen[1]) {
		if(!(mState == State::Closing || mState == State::Opening)) {
			player->GetHUD()->SetAction(player->mKeyUse, player->GetLocalization()->GetString("openGateway"));
			if(player->IsUseButtonHit() && !mLocked) {
				mButtonSound->SetPosition(player->mNearestPickedNode->GetPosition());
				if(mState != State::Opened) {
					player->mNearestPickedNode->GetCurrentAnimation()->Rewind();
					player->mNearestPickedNode->GetCurrentAnimation()->SetEnabled(true);
				}
			}
		}
	}
	if(player->mNearestPickedNode == mButtonClose[0] || player->mNearestPickedNode == mButtonClose[1]) {
		if(!(mState == State::Closing || mState == State::Opening)) {
			player->GetHUD()->SetAction(player->mKeyUse, player->GetLocalization()->GetString("closeGateway"));
			if(player->IsUseButtonHit() && !mLocked) {
				mButtonSound->SetPosition(player->mNearestPickedNode->GetPosition());
				if(mState != State::Closed) {
					player->mNearestPickedNode->GetCurrentAnimation()->Rewind();
					player->mNearestPickedNode->GetCurrentAnimation()->SetEnabled(true);
				}
			}
		}
	}
}

void Gate::Open() {
	mGate->SetAnimation(&mOpenAnim);
	mOpenAnim.Rewind();
	mOpenAnim.SetEnabled(true);
}

void Gate::Close() {
	mGate->SetAnimation(&mCloseAnim);
	mCloseAnim.Rewind();
	mCloseAnim.SetEnabled(true);
}

shared_ptr<ISceneNode> Gate::GetNode() const {
	return mGate;
}

Gate::State Gate::GetState() const {
	return mState;
}

Gate::Gate(shared_ptr<ISceneNode> gate, shared_ptr<ISceneNode> buttonOpen, shared_ptr<ISceneNode> buttonClose, shared_ptr<ISceneNode> buttonOpen2, shared_ptr<ISceneNode> buttonClose2) {
	mGate = gate;

	mLocked = false;

	int frameCount = mGate->GetTotalAnimationFrameCount();

	mOpenAnim = Animation(0, frameCount / 2, 3);
	mOpenAnim.AddFrameListener(0, [this] { mBeginSound->Play(); mState = State::Opening; });
	mOpenAnim.AddFrameListener(2, [this] { mIdleSound->Play(); });
	mOpenAnim.AddFrameListener(frameCount / 2, [this] { mEndSound->Play(); mState = State::Opened; mIdleSound->Stop(); });

	mCloseAnim = Animation(frameCount / 2, frameCount, 3);
	mCloseAnim.AddFrameListener(frameCount / 2, [this] {mBeginSound->Play(); mState = State::Closing; });
	mCloseAnim.AddFrameListener(frameCount / 2 + 2, [this] { mIdleSound->Play(); });
	mCloseAnim.AddFrameListener(frameCount - 2, [this] { mEndSound->Play();	mState = State::Closed;	mIdleSound->Stop(); });

	for(int i = 0; i < 4; i++) {
		mButtonPushAnim[i] = Animation(0, frameCount, 0.1);
		mButtonPushAnim[i].AddFrameListener(frameCount / 2, [this] { mButtonSound->Play(); });
	}

	mButtonClose[0] = buttonClose;
	mButtonClose[0]->SetAnimation(&mButtonPushAnim[0]);
	mButtonPushAnim[0].AddFrameListener(frameCount / 2, [this] { Close(); });

	mButtonOpen[0] = buttonOpen;
	mButtonOpen[0]->SetAnimation(&mButtonPushAnim[1]);
	mButtonPushAnim[1].AddFrameListener(frameCount / 2, [this] { Open(); });

	mButtonClose[1] = buttonClose2;
	mButtonClose[1]->SetAnimation(&mButtonPushAnim[2]);
	mButtonPushAnim[2].AddFrameListener(frameCount / 2, [this] { Close(); });

	mButtonOpen[1] = buttonOpen2;
	mButtonOpen[1]->SetAnimation(&mButtonPushAnim[3]);
	mButtonPushAnim[3].AddFrameListener(frameCount / 2, [this] { Open(); });

	mState = State::Closed;

	auto soundSystem = gate->GetFactory()->GetEngineInterface()->GetSoundSystem();

	mBeginSound = soundSystem->LoadSound3D("data/sounds/door_open_start.ogg");
	mBeginSound->Attach(mGate);

	mIdleSound = soundSystem->LoadSound3D("data/sounds/door_open_idle.ogg");
	mIdleSound->Attach(mGate);
	mIdleSound->SetLoop(true);

	mEndSound = soundSystem->LoadSound3D("data/sounds/door_open_end.ogg");
	mEndSound->Attach(mGate);

	mButtonSound = soundSystem->LoadSound3D("data/sounds/button.ogg");
}

Gate::~Gate() {

}
