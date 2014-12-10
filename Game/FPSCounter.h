#pragma once

#include "Game.h"

class FPSCounter
{
public:
    ruTimerHandle timer;
    int frameCounter;
    int fps;

    FPSCounter();

    void RegisterFrame();
};