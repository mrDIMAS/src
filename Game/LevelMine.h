#pragma once

#include "LightAnimator.h"
#include "Level.h"
#include "Enemy.h"
#include "ItemPlace.h"
#include "AmbientSoundSet.h"

class LevelMine : public Level {
private:
    void UpdateExplodeSequence();
    void CleanUpExplodeArea();
    void CreateItems();

    NodeHandle screamerZone;
    NodeHandle screamerZone2;
    NodeHandle concreteWall;
    NodeHandle deathZone;
    NodeHandle detonator;
    NodeHandle findItemsZone;
    NodeHandle newLevelZone;
    NodeHandle stoneFallZone;
    NodeHandle wireModels[4];
    NodeHandle detonatorModels[4];
    NodeHandle explosivesModels[4];

    Item * explosives[4];
    Item * detonators[4];
    Item * wires[4];
    Item * fuel[2];

    ItemPlace * detonatorPlace[4];

    SoundHandle alertSound;
    SoundHandle explosionSound;

    TimerHandle explosionTimer;
    TimerHandle beepSoundTimer;

    bool detonatorActivated;

    int readyExplosivesCount;

    float beepSoundTiming;

    LightAnimator * explosionFlashAnimator;
public:
    explicit LevelMine( );
    virtual ~LevelMine();
    virtual void DoScenario() final;
    virtual void Show() final;
    virtual void Hide() final;
    virtual void OnSerialize( TextFileStream & out ) final;
    virtual void OnDeserialize( TextFileStream & in ) final;
};