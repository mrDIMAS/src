#pragma once

#include "Level.h"
#include "Enemy.h"
#include "Pathfinder.h"

class TestingChamber : public Level {
public:
    vector< GraphVertex* > path;

    TestingChamber( );
    ~TestingChamber();

    Enemy * enemy;
    virtual void DoScenario();
    virtual void Show();
    virtual void Hide();

    virtual void OnSerialize( SaveFile & out ) final {

    }

    virtual void OnDeserialize( SaveFile & in ) final {

    }
};