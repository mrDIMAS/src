#pragma  once

#include "game.h"

class ScreenScreamer {
public:
    ruSoundHandle sound;
    int periods;
    ruTextureHandle tex;
    int nextPeriodFrames;
    int pauseFrames;
    ScreenScreamer();
    void DoPeriods( int p );
    void Update( );
};