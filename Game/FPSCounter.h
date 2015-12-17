#pragma once

#include "Game.h"

class FPSCounter {
public:
    ruTimer * timer;
    int frameCounter;
    int fps;

    FPSCounter();

    void RegisterFrame();
};