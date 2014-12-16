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

    virtual void OnSerialize( TextFileStream & out ) final {

    }

    virtual void OnDeserialize( TextFileStream & in ) final {

    }
};