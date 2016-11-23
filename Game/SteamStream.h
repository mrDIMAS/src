#pragma once

#include "Game.h"

class SteamStream {
private:
	shared_ptr<IParticleSystem > mSteam;
	shared_ptr<ISound> mSound;
	float mPower;
public:
	SteamStream(shared_ptr<ISceneNode> obj, Vector3 speedMin, Vector3 speedMax, shared_ptr<ISound> hissSound);
	void Update();
	~SteamStream();
	void SetPower(float power)
	{
		mPower = power;
	}
};