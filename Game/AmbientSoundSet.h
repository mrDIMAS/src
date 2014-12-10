#pragma once

#include "Game.h"


class AmbientSoundSet
{
private:
    vector< ruSoundHandle > sounds;

    float timeToNextSoundSec;

    float timeMinSec;
    float timeMaxSec;

    ruTimerHandle timer;
public:

    AmbientSoundSet();

    void SetMinMax( float tMin, float tMax );
    void AddSound( ruSoundHandle sound );
    void DoRandomPlaying();
};