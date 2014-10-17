#pragma once

#include "Game.h"

class SteamStream {
public:
    NodeHandle ps;
    SoundHandle snd;
    float power;

    SteamStream( NodeHandle obj, Vector3 speedMin, Vector3 speedMax, SoundHandle hissSound );
    void Update();
};