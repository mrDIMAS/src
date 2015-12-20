#pragma once

#include "Game.h"

class Fan {
private:
    shared_ptr<ruSceneNode> mBody;
    float mTurnSpeed;
    ruVector3 mTurnAxis;
    float mAngle;
    shared_ptr<ruSound> mEngineSound;
public:
    explicit Fan( shared_ptr<ruSceneNode> object, float turnSpeed, ruVector3 turnAxis, shared_ptr<ruSound> engineSound );
    virtual ~Fan();
    void DoTurn();
};
