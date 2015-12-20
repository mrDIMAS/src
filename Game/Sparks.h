#pragma once

#include "Game.h"

class Sparks {
private:
    shared_ptr<ruParticleSystem > mParticleSystem;
    bool mAlive;
    shared_ptr<ruSound> mSound;
public:
    Sparks( shared_ptr<ruSceneNode> at, shared_ptr<ruSound> emits );
	~Sparks();
    void Update();
	bool IsAlive();
};