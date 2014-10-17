#pragma once

#include "Game.h"


class AmbientSoundSet {
private:
    vector< SoundHandle > sounds;

    float timeToNextSoundSec;

    float timeMinSec;
    float timeMaxSec;

    TimerHandle timer;
public:

    AmbientSoundSet();

    void SetMinMax( float tMin, float tMax );
    void AddSound( SoundHandle sound );
    void DoRandomPlaying();
};