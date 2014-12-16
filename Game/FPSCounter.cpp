#include "FPSCounter.h"




void FPSCounter::RegisterFrame() {
    frameCounter++;

    if( ruGetElapsedTimeInSeconds( timer ) >= 1.0f ) {
        fps = frameCounter;

        frameCounter = 0;

        ruRestartTimer( timer );
    }
}

FPSCounter::FPSCounter() {
    timer = ruCreateTimer();

    fps = 0;
    frameCounter = 0;
}
