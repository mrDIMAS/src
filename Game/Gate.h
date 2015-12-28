#pragma once

#include "Game.h"
#include "Player.h"

class Gate {
private:
	enum class State {
		Opening,
		Closing,
		Opened,
		Closed,
	};
	State mState;
	shared_ptr<ruSceneNode> mGate;
	shared_ptr<ruSceneNode> mButtonOpen[2];
	shared_ptr<ruSceneNode> mButtonClose[2];
	shared_ptr<ruSound> mBeginSound;
	shared_ptr<ruSound> mIdleSound;
	shared_ptr<ruSound> mEndSound;
	shared_ptr<ruSound> mButtonSound;	

	ruAnimation mOpenAnim;
	ruAnimation mCloseAnim;
	ruAnimation mButtonPushAnim[4];

	void Proxy_ButtonPush() {
		mButtonSound->Play();
	}

	void Proxy_BeginGateClosing() {
		mGate->SetAnimation( &mCloseAnim );
		mCloseAnim.Rewind();
		mCloseAnim.SetEnabled( true );
	}

	void Proxy_BeginGateOpening() {
		mGate->SetAnimation( &mOpenAnim );
		mOpenAnim.Rewind();
		mOpenAnim.SetEnabled( true );
	}

	void Proxy_Opening() {
		mBeginSound->Play();
		mState = State::Opening;
	}

	void Proxy_Opened() {
		mEndSound->Play();
		mState = State::Opened;
		mIdleSound->Stop();
	}

	void Proxy_Idle() {
		mIdleSound->Play();
	}

	void Proxy_Closed() {
		mEndSound->Play();
		mState = State::Closed;
		mIdleSound->Stop();
	}

	void Proxy_Closing() {
		mBeginSound->Play();
		mState = State::Closing;
	}
public:
	explicit Gate( shared_ptr<ruSceneNode> gate, shared_ptr<ruSceneNode> buttonOpen, shared_ptr<ruSceneNode> buttonClose, shared_ptr<ruSceneNode> buttonOpen2, shared_ptr<ruSceneNode> buttonClose2  );
	void Update();
};