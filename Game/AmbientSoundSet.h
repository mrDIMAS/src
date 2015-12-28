#pragma once

#include "Game.h"


class AmbientSoundSet {
private:
    vector< shared_ptr<ruSound> > mSoundList;
    float mTimeToNextSoundSec;
    float mTimeMinSec;
    float mTimeMaxSec;
    shared_ptr<ruTimer> mTimer;
public:
    AmbientSoundSet();
	~AmbientSoundSet();
    void SetMinMax( float tMin, float tMax );
    void AddSound( shared_ptr<ruSound> sound );
    void DoRandomPlaying();
};