#pragma once

#include "Way.h"

class Ladder : public Way {
private:
    void DoLeave();
    FootstepsType footsteps;
public:
    Ladder( NodeHandle hBegin, NodeHandle hEnd, NodeHandle hEnterZone, NodeHandle hBeginLeavePoint, NodeHandle hEndLeavePoint );

    virtual void LookAtTarget() final;
    virtual void DoPlayerCrawling() final;
    virtual void Enter() final;
    virtual void SetDirection( Direction direction ) final;
};