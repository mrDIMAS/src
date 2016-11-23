#pragma once

#include "Game.h"

class FPSCounter {
public:
	shared_ptr<ITimer> timer;
	int frameCounter;
	int fps;

	FPSCounter();

	void RegisterFrame();
};