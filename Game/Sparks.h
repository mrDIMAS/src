#pragma once

#include "Game.h"

class Sparks {
public:
    ruSceneNode ps;
    int alive;
    ruSound es;
public:

    Sparks( ruSceneNode at, ruSound emits );
	~Sparks() {
		es.Free();
	}
    void Update();
};