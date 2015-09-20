#pragma once

#include "Game.h"

class SteamStream {
public:
    ruSceneNode ps;
    ruSound snd;
    float power;

    SteamStream( ruSceneNode obj, ruVector3 speedMin, ruVector3 speedMax, ruSound hissSound );
    void Update();
};