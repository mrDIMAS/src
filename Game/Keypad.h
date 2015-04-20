#pragma once

#include "Game.h"
#include "Door.h"
#include "Player.h"

class Keypad {
private:
	ruNodeHandle mKeypad;
	ruNodeHandle mKeys[10];
	ruNodeHandle mKeyCancel;
	Door * mDoorToUnlock;
	string mCurrentCode;
	string mCodeToUnlock;
	bool mKeyState[10];
	ruVector3 mKeysInitialPosition[10];
	ruVector3 mKeysPressedOffsets[10];
	ruSoundHandle mButtonPushSound;
	ruSoundHandle mButtonPopSound;

	void Reset();
public:
	explicit Keypad( ruNodeHandle keypad, ruNodeHandle key0, ruNodeHandle key1, ruNodeHandle key2, ruNodeHandle key3, 
		ruNodeHandle key4, ruNodeHandle key5, ruNodeHandle key6, ruNodeHandle key7, ruNodeHandle key8, ruNodeHandle key9,
		ruNodeHandle keyCancel, Door * doorToUnlock, string codeToUnlock );

	void Update();
};