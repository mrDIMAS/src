#pragma once

#include "Game.h"

class SteamStream {
public:
    ruNodeHandle ps;
    ruSoundHandle snd;
    float power;

    SteamStream( ruNodeHandle obj, ruVector3 speedMin, ruVector3 speedMax, ruSoundHandle hissSound );
    void Update();
};