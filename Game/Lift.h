#pragma once

#include "Game.h"

class Lift {
private:
    ruNodeHandle body;
    ruNodeHandle scr;
    ruNodeHandle pos;
    ruNodeHandle sourcePoint;
    ruNodeHandle destPoint;
    ruNodeHandle target;

    bool arrived;

    ruSoundHandle motorIdleSound;
public:
    Lift( ruNodeHandle object, ruNodeHandle screen, ruNodeHandle src, ruNodeHandle dest, ruSoundHandle motorIS );
    void Update();
};