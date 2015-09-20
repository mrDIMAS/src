#pragma once

#include "Game.h"


class AmbientSoundSet {
private:
    vector< ruSound > sounds;

    float timeToNextSoundSec;

    float timeMinSec;
    float timeMaxSec;

    ruTimerHandle timer;
public:

    AmbientSoundSet();

    void SetMinMax( float tMin, float tMax );
    void AddSound( ruSound sound );
    void DoRandomPlaying();
};