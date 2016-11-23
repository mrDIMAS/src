#pragma once

#include "Game.h"


class AmbientSoundSet {
private:
	vector< shared_ptr<ISound> > mSoundList;
	float mTimeToNextSoundSec;
	float mTimeMinSec;
	float mTimeMaxSec;
	shared_ptr<ITimer> mTimer;
public:
	AmbientSoundSet();
	~AmbientSoundSet();
	void SetMinMax(float tMin, float tMax);
	void AddSound(shared_ptr<ISound> sound);
	void DoRandomPlaying();
};