#pragma once

#include "Game.h"

class Sparks {
public:
    ruNodeHandle ps;
    int alive;
    ruSoundHandle es;
public:

    Sparks( ruNodeHandle at, ruSoundHandle emits );
    void Update();
};