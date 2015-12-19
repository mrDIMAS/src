#pragma once

#include "Game.h"

class SteamStream {
private:
	shared_ptr<ruParticleSystem > mSteam;
	ruSound mSound;
	float mPower;
public:
    SteamStream( shared_ptr<ruSceneNode> obj, ruVector3 speedMin, ruVector3 speedMax, ruSound hissSound );
    void Update();
	~SteamStream();
	void SetPower( float power ) {
		mPower = power;
	}
};