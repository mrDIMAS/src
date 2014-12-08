#pragma once

#include "Game.h"

class Fan {
public:
    ruNodeHandle body;
    float turnSpeed;
    ruVector3 tAxis;
    float angle;
    ruSoundHandle sound;

    Fan( ruNodeHandle obj, float ts, ruVector3 axis, ruSoundHandle snd );
    void DoTurn();
};
