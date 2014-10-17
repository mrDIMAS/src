#pragma once

#include "Game.h"
#include "GUI.h"

// Performs animation of goal text and it's rendering

class Goal {
private:
    TimerHandle waitTimer;

    float currentY;
    float destY;
    float initialY;
    float waitSec;

    float alpha;

    string text;
public:
    Goal();
    void SetText( string t );
    void AnimateAndRender();
};