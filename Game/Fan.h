#pragma once

#include "Game.h"

class Fan {
private:
    ruNodeHandle mBody;
    float mTurnSpeed;
    ruVector3 mTurnAxis;
    float mAngle;
    ruSoundHandle mEngineSound;
public:
    explicit Fan( ruNodeHandle object, float turnSpeed, ruVector3 turnAxis, ruSoundHandle engineSound );
    virtual ~Fan();
    void DoTurn();
};
