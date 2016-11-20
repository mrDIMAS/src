#include "Precompiled.h"

#include "FPSCounter.h"

void FPSCounter::RegisterFrame()
{
	frameCounter++;
	if(timer->GetElapsedTimeInSeconds() >= 1.0f) {
		fps = frameCounter;
		frameCounter = 0;
		timer->Restart();
	}
}

FPSCounter::FPSCounter()
{
	timer = ruTimer::Create();

	fps = 0;
	frameCounter = 0;
}
