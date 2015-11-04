#pragma once

#include "Game.h"
#include "Door.h"
#include "Player.h"

class Keypad {
private:
	ruSceneNode mKeypad;
	ruSceneNode mKeys[10];
	ruSceneNode mKeyCancel;
	Door * mDoorToUnlock;
	string mCurrentCode;
	string mCodeToUnlock;
	bool mKeyState[10];
	ruVector3 mKeysInitialPosition[10];
	ruVector3 mKeysPressedOffsets[10];
	ruSound mButtonPushSound;
	ruSound mButtonPopSound;

	void Reset();
public:
	explicit Keypad( ruSceneNode keypad, ruSceneNode key0, ruSceneNode key1, ruSceneNode key2, ruSceneNode key3, 
		ruSceneNode key4, ruSceneNode key5, ruSceneNode key6, ruSceneNode key7, ruSceneNode key8, ruSceneNode key9,
		ruSceneNode keyCancel, Door * doorToUnlock, string codeToUnlock );
	~Keypad() {
		mButtonPushSound.Free();
		mButtonPopSound.Free();
	}
	void Update();
};