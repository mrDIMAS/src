#pragma once

#include "Way.h"

class Ladder : public Way {
private:
    void DoLeave();
    FootstepsType footsteps;
public:
    Ladder( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint );

    virtual void LookAtTarget() final;
    virtual void DoPlayerCrawling() final;
    virtual void Enter() final;
    virtual void SetDirection( Direction direction ) final;
};