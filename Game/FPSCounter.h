#pragma once

#include "Game.h"

class FPSCounter {
public:
	shared_ptr<ruTimer> timer;
	int frameCounter;
	int fps;

	FPSCounter();

	void RegisterFrame();
};