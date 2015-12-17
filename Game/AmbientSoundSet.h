#pragma once

#include "Game.h"


class AmbientSoundSet {
private:
    vector< ruSound > mSoundList;
    float mTimeToNextSoundSec;
    float mTimeMinSec;
    float mTimeMaxSec;
    ruTimer * mTimer;
public:
    AmbientSoundSet();
	~AmbientSoundSet();
    void SetMinMax( float tMin, float tMax );
    void AddSound( ruSound sound );
    void DoRandomPlaying();
};