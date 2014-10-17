#pragma once

#include "Game.h"

class Fan {
public:
    NodeHandle body;
    float turnSpeed;
    Vector3 tAxis;
    float angle;
    SoundHandle sound;

    Fan( NodeHandle obj, float ts, Vector3 axis, SoundHandle snd );
    void DoTurn();
};
