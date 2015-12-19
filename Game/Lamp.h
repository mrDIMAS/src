#pragma once

#include "Game.h"

class Lamp {
private:
	void PlaySounds();
public:
	ruSound mBuzzSound;
	shared_ptr<ruSceneNode> mLamp;

	explicit Lamp( shared_ptr<ruSceneNode> lamp, ruSound buzz );

	void Hide();

	void Show( );

	void Update();
};