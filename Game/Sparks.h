#pragma once

#include "Game.h"

class Sparks {
private:
	shared_ptr<IParticleSystem > mParticleSystem;
	bool mAlive;
	shared_ptr<ISound> mSound;
public:
	Sparks(shared_ptr<ISceneNode> at, shared_ptr<ISound> emits);
	~Sparks();
	void Update();
	bool IsAlive();
};