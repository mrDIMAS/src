#pragma once

#include "Game.h"
#include "Door.h"
#include "Player.h"

class Keypad {
private:
	shared_ptr<ISceneNode> mKeypad;
	shared_ptr<ISceneNode> mKeys[10];
	shared_ptr<ISceneNode> mKeyCancel;
	weak_ptr<Door> mDoorToUnlock;
	string mCurrentCode;
	string mCodeToUnlock;
	bool mKeyState[10];
	Vector3 mKeysInitialPosition[10];
	Vector3 mKeysPressedOffsets[10];
	shared_ptr<ISound> mButtonPushSound;
	shared_ptr<ISound> mButtonPopSound;
public:
	void Reset();
	void Update();
	explicit Keypad(shared_ptr<ISceneNode> keypad, shared_ptr<ISceneNode> key0, shared_ptr<ISceneNode> key1, shared_ptr<ISceneNode> key2, shared_ptr<ISceneNode> key3,
		shared_ptr<ISceneNode> key4, shared_ptr<ISceneNode> key5, shared_ptr<ISceneNode> key6, shared_ptr<ISceneNode> key7, shared_ptr<ISceneNode> key8, shared_ptr<ISceneNode> key9,
		shared_ptr<ISceneNode> keyCancel, weak_ptr<Door> doorToUnlock, string codeToUnlock);
	~Keypad();
};