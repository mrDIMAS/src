#pragma once

#include "Game.h"

class Valve {
public:
    ruNodeHandle object;
    ruVector3 tAxis;

    float angle;
    bool done;
    int tc;

    float value;

    Valve( ruNodeHandle obj, ruVector3 axis, int turnCount = 2 );
    void Update();
};
