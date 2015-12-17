#pragma once

#include "Game.h"

class SteamStream {
private:
	ruParticleSystem * mSteam;
	ruSound mSound;
	float mPower;
public:
    SteamStream( ruSceneNode * obj, ruVector3 speedMin, ruVector3 speedMax, ruSound hissSound );
    void Update();
	~SteamStream();
	void SetPower( float power ) {
		mPower = power;
	}
};