#pragma once

#include "Game.h"

class Lamp {
private:
	void PlaySounds();
public:
	ruSound mBuzzSound;
	ruSceneNode * mLamp;

	explicit Lamp( ruSceneNode * lamp, ruSound buzz );

	void Hide();

	void Show( );

	void Update();
};