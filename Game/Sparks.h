#pragma once

#include "Game.h"

class Sparks {
private:
    shared_ptr<ruParticleSystem > mParticleSystem;
    bool mAlive;
    ruSound mSound;
public:
    Sparks( shared_ptr<ruSceneNode> at, ruSound emits );
	~Sparks();
    void Update();
	bool IsAlive();
};