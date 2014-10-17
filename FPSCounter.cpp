#include "FPSCounter.h"

void FPSCounter::RegisterFrame() {
    frameCounter++;

    if( timer->GetElapsedTimeInSeconds() > 1.0f ) {
        fps = frameCounter;

        frameCounter = 0;

        timer->RestartTimer();
    }
}

FPSCounter::FPSCounter() {
    timer = new Timer();

    fps = 60;
    frameCounter = 0;
}
