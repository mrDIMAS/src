#pragma once

#include "Game.h"

class Lamp {
private:
	void PlaySounds();
public:
	shared_ptr<ruSound> mBuzzSound;
	shared_ptr<ruSceneNode> mLamp;
	explicit Lamp( shared_ptr<ruSceneNode> lamp, shared_ptr<ruSound> buzz );
	void Hide();
	void Show( );
	void Update();
};