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

	shared_ptr<ruSceneNode> mGate;
	float mGateHeight;
	shared_ptr<ruSceneNode> mButtonOpen[2];
	shared_ptr<ruSceneNode> mButtonClose[2];
	ruVector3 mInitialPosition;
	State mState;
	float mGateYOffset;
	shared_ptr<ruSound> mBeginSound;
	shared_ptr<ruSound> mIdleSound;
	shared_ptr<ruSound> mEndSound;
	shared_ptr<ruSound> mButtonSound;
	
public:
	explicit Gate( shared_ptr<ruSceneNode> gate, shared_ptr<ruSceneNode> buttonOpen, shared_ptr<ruSceneNode> buttonClose, shared_ptr<ruSceneNode> buttonOpen2, shared_ptr<ruSceneNode> buttonClose2  );
	void OnEndMoving();
	void Update();
};