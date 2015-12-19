#pragma once

#include "Way.h"

class Ladder : public Way {
private:
    void DoLeave();
public:
    explicit Ladder( shared_ptr<ruSceneNode> hBegin, shared_ptr<ruSceneNode> hEnd, shared_ptr<ruSceneNode> hEnterZone, shared_ptr<ruSceneNode> hBeginLeavePoint, shared_ptr<ruSceneNode> hEndLeavePoint );

    virtual void DoPlayerCrawling() final;
    virtual void Enter() final;
    virtual void SetDirection( Direction direction ) final;
};