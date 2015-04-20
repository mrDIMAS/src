#pragma once

#include "Game.h"

class Lamp {
private:
	void PlaySounds();
public:
	ruSoundHandle mBuzzSound;
	ruNodeHandle mLamp;

	explicit Lamp( ruNodeHandle lamp, ruSoundHandle buzz );

	void Hide();

	void Show( );

	void Update();
};