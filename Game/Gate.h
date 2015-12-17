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

	ruSceneNode * mGate;
	float mGateHeight;
	ruSceneNode * mButtonOpen[2];
	ruSceneNode * mButtonClose[2];
	ruVector3 mInitialPosition;
	State mState;
	float mGateYOffset;
	ruSound mBeginSound;
	ruSound mIdleSound;
	ruSound mEndSound;
	ruSound mButtonSound;
	
public:
	explicit Gate( ruSceneNode * gate, ruSceneNode * buttonOpen, ruSceneNode * buttonClose, ruSceneNode * buttonOpen2, ruSceneNode * buttonClose2  );
	void OnEndMoving();
	void Update();
};