#pragma once

#include "Game.h"
#include "Door.h"
#include "Player.h"

class Keypad {
private:
	shared_ptr<ruSceneNode> mKeypad;
	shared_ptr<ruSceneNode> mKeys[10];
	shared_ptr<ruSceneNode> mKeyCancel;
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
	explicit Keypad( shared_ptr<ruSceneNode> keypad, shared_ptr<ruSceneNode> key0, shared_ptr<ruSceneNode> key1, shared_ptr<ruSceneNode> key2, shared_ptr<ruSceneNode> key3, 
		shared_ptr<ruSceneNode> key4, shared_ptr<ruSceneNode> key5, shared_ptr<ruSceneNode> key6, shared_ptr<ruSceneNode> key7, shared_ptr<ruSceneNode> key8, shared_ptr<ruSceneNode> key9,
		shared_ptr<ruSceneNode> keyCancel, Door * doorToUnlock, string codeToUnlock );
	~Keypad() {
		mButtonPushSound.Free();
		mButtonPopSound.Free();
	}
	void Update();
};