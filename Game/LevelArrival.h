#pragma once

#include "Level.h"
#include "LightAnimator.h"
#include "Fire.h"
#include "AmbientSoundSet.h"

class LevelArrival : public Level {
public:
    // Sounds
    SoundHandle windSound;
    SoundHandle generatorSound;
    SoundHandle explosionSound;
    SoundHandle strangeSound;

    // Zones
    NodeHandle strangeSoundZone;
    NodeHandle rocksFallZone;
    NodeHandle nextLevelLoadZone;

    // Entities
    NodeHandle rocks;
    NodeHandle rocksPos;
    NodeHandle generator;

    NodeHandle lamp1;
    NodeHandle lamp2;

    LevelArrival( );
    ~LevelArrival( );

    virtual void DoScenario();
    virtual void Show();
    virtual void Hide();

    virtual void OnSerialize( TextFileStream & out ) final {

    }

    virtual void OnDeserialize( TextFileStream & in ) final {

    }
};