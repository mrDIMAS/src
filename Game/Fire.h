#pragma once

#include "Game.h"

class Fire {
public:
    ruNodeHandle particleSystem;
    ruNodeHandle light;

    Fire( float size, float height );

    ~Fire();
};