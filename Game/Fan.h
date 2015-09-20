#pragma once

#include "Game.h"

class Fan {
private:
    ruSceneNode mBody;
    float mTurnSpeed;
    ruVector3 mTurnAxis;
    float mAngle;
    ruSound mEngineSound;
public:
    explicit Fan( ruSceneNode object, float turnSpeed, ruVector3 turnAxis, ruSound engineSound );
    virtual ~Fan();
    void DoTurn();
};
