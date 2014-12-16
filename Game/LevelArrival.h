#pragma once

#include "Level.h"
#include "LightAnimator.h"
#include "AmbientSoundSet.h"

class LevelArrival : public Level {
public:
    // Sounds
    ruSoundHandle windSound;
    ruSoundHandle generatorSound;
    ruSoundHandle explosionSound;
    ruSoundHandle strangeSound;

    // Zones
    ruNodeHandle strangeSoundZone;
    ruNodeHandle rocksFallZone;
    ruNodeHandle nextLevelLoadZone;

    // Entities
    ruNodeHandle rocks;
    ruNodeHandle rocksPos;
    ruNodeHandle generator;

    ruNodeHandle lamp1;
    ruNodeHandle lamp2;

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