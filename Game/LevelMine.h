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

    ruNodeHandle screamerZone;
    ruNodeHandle screamerZone2;
    ruNodeHandle concreteWall;
    ruNodeHandle deathZone;
    ruNodeHandle detonator;
    ruNodeHandle findItemsZone;
    ruNodeHandle newLevelZone;
    ruNodeHandle stoneFallZone;
    ruNodeHandle wireModels[4];
    ruNodeHandle detonatorModels[4];
    ruNodeHandle explosivesModels[4];

    Item * explosives[4];
    Item * detonators[4];
    Item * wires[4];
    Item * fuel[2];

    ItemPlace * detonatorPlace[4];

    ruSoundHandle alertSound;
    ruSoundHandle explosionSound;

    ruTimerHandle explosionTimer;
    ruTimerHandle beepSoundTimer;

    bool detonatorActivated;

    int readyExplosivesCount;

    float beepSoundTiming;

    LightAnimator * explosionFlashAnimator;

	Enemy * enemy;
public:
    explicit LevelMine( );
    virtual ~LevelMine();
    virtual void DoScenario() final;
    virtual void Show() final;
    virtual void Hide() final;
    virtual void OnSerialize( TextFileStream & out ) final;
    virtual void OnDeserialize( TextFileStream & in ) final;
};