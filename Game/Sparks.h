#pragma once

#include "Game.h"

class Sparks {
private:
    ruParticleSystem * mParticleSystem;
    bool mAlive;
    ruSound mSound;
public:
    Sparks( ruSceneNode * at, ruSound emits );
	~Sparks();
    void Update();
	bool IsAlive();
};