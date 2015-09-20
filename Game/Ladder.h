#pragma once

#include "Way.h"

class Ladder : public Way {
private:
    void DoLeave();
    FootstepsType mFootsteps;
public:
    explicit Ladder( ruSceneNode hBegin, ruSceneNode hEnd, ruSceneNode hEnterZone, ruSceneNode hBeginLeavePoint, ruSceneNode hEndLeavePoint );

    virtual void DoPlayerCrawling() final;
    virtual void Enter() final;
    virtual void SetDirection( Direction direction ) final;
};