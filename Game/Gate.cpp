#include "Precompiled.h"
#include "Gate.h"
#include "Level.h"

void Gate::Proxy_ButtonPush() {
	mButtonSound->Play();
}

void Gate::Proxy_BeginGateClosing() {
	mGate->SetAnimation(&mCloseAnim);

	mCloseAnim.Rewind();
	mCloseAnim.SetEnabled(true);
}

void Gate::Proxy_BeginGateOpening() {
	mGate->SetAnimation(&mOpenAnim);
	mOpenAnim.Rewind();
	mOpenAnim.SetEnabled(true);
}

void Gate::Proxy_Opening() {
	mBeginSound->Play();
	mState = State::Opening;
}

void Gate::Proxy_Opened() {
	mEndSound->Play();
	mState = State::Opened;
	mIdleSound->Stop();
}

void Gate::Proxy_Idle() {
	mIdleSound->Play();
}

void Gate::Proxy_Closed() {
	mEndSound->Play();
	mState = State::Closed;
	mIdleSound->Stop();
}

void Gate::Proxy_Closing() {
	mBeginSound->Play();
	mState = State::Closing;
}

void Gate::Update() {
	mOpenAnim.Update();
	mCloseAnim.Update();

	for (int i = 0; i < 4; i++) {
		mButtonPushAnim[i].Update();
	}

	auto & player = Level::Current()->GetPlayer();

	if (player->mNearestPickedNode == mButtonOpen[0] || player->mNearestPickedNode == mButtonOpen[1]) {
		if (!(mState == State::Closing || mState == State::Opening)) {
			player->GetHUD()->SetAction(player->mKeyUse, player->GetLocalization()->GetString("openGateway"));
			if (ruInput::IsKeyHit(player->mKeyUse)) {
				mButtonSound->SetPosition(player->mNearestPickedNode->GetPosition());
				if (mState != State::Opened) {
					player->mNearestPickedNode->GetCurrentAnimation()->Rewind();
					player->mNearestPickedNode->GetCurrentAnimation()->SetEnabled(true);
				}
			}
		}
	}
	if (player->mNearestPickedNode == mButtonClose[0] || player->mNearestPickedNode == mButtonClose[1]) {
		if (!(mState == State::Closing || mState == State::Opening)) {
			player->GetHUD()->SetAction(player->mKeyUse, player->GetLocalization()->GetString("closeGateway"));
			if (ruInput::IsKeyHit(player->mKeyUse)) {
				mButtonSound->SetPosition(player->mNearestPickedNode->GetPosition());
				if (mState != State::Closed) {
					player->mNearestPickedNode->GetCurrentAnimation()->Rewind();
					player->mNearestPickedNode->GetCurrentAnimation()->SetEnabled(true);
				}
			}
		}
	}
}

void Gate::Open() {
	Proxy_BeginGateOpening();
}

shared_ptr<ruSceneNode> Gate::GetNode() const {
	return mGate;
}

Gate::State Gate::GetState() const {
	return mState;
}

Gate::Gate(shared_ptr<ruSceneNode> gate, shared_ptr<ruSceneNode> buttonOpen, shared_ptr<ruSceneNode> buttonClose, shared_ptr<ruSceneNode> buttonOpen2, shared_ptr<ruSceneNode> buttonClose2) {
	mGate = gate;

	int frameCount = mGate->GetTotalAnimationFrameCount();

	mOpenAnim = ruAnimation(0, frameCount / 2, 3);
	mOpenAnim.AddFrameListener(0, ruDelegate::Bind(this, &Gate::Proxy_Opening));
	mOpenAnim.AddFrameListener(2, ruDelegate::Bind(this, &Gate::Proxy_Idle));
	mOpenAnim.AddFrameListener(frameCount / 2, ruDelegate::Bind(this, &Gate::Proxy_Opened));

	mCloseAnim = ruAnimation(frameCount / 2, frameCount, 3);
	mCloseAnim.AddFrameListener(frameCount / 2, ruDelegate::Bind(this, &Gate::Proxy_Closing));
	mCloseAnim.AddFrameListener(frameCount / 2 + 2, ruDelegate::Bind(this, &Gate::Proxy_Idle));
	mCloseAnim.AddFrameListener(frameCount - 2, ruDelegate::Bind(this, &Gate::Proxy_Closed));

	for (int i = 0; i < 4; i++) {
		mButtonPushAnim[i] = ruAnimation(0, frameCount, 0.1);
		mButtonPushAnim[i].AddFrameListener(frameCount / 2, ruDelegate::Bind(this, &Gate::Proxy_ButtonPush));
	}

	mButtonClose[0] = buttonClose;
	mButtonClose[0]->SetAnimation(&mButtonPushAnim[0]);
	mButtonPushAnim[0].AddFrameListener(frameCount / 2, ruDelegate::Bind(this, &Gate::Proxy_BeginGateClosing));

	mButtonOpen[0] = buttonOpen;
	mButtonOpen[0]->SetAnimation(&mButtonPushAnim[1]);
	mButtonPushAnim[1].AddFrameListener(frameCount / 2, ruDelegate::Bind(this, &Gate::Proxy_BeginGateOpening));

	mButtonClose[1] = buttonClose2;
	mButtonClose[1]->SetAnimation(&mButtonPushAnim[2]);
	mButtonPushAnim[2].AddFrameListener(frameCount / 2, ruDelegate::Bind(this, &Gate::Proxy_BeginGateClosing));

	mButtonOpen[1] = buttonOpen2;
	mButtonOpen[1]->SetAnimation(&mButtonPushAnim[3]);
	mButtonPushAnim[3].AddFrameListener(frameCount / 2, ruDelegate::Bind(this, &Gate::Proxy_BeginGateOpening));

	mState = State::Closed;

	mBeginSound = ruSound::Load3D("data/sounds/door_open_start.ogg");
	mBeginSound->Attach(mGate);

	mIdleSound = ruSound::Load3D("data/sounds/door_open_idle.ogg");
	mIdleSound->Attach(mGate);
	mIdleSound->SetLoop(true);

	mEndSound = ruSound::Load3D("data/sounds/door_open_end.ogg");
	mEndSound->Attach(mGate);

	mButtonSound = ruSound::Load3D("data/sounds/button.ogg");
}

Gate::~Gate() {

}
