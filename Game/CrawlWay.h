#pragma once

#include "Way.h"


class CrawlWay : public Way
{
private:
    void DoLeave();
public:

    explicit CrawlWay( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint );
    virtual ~CrawlWay();
    virtual void DoPlayerCrawling() final;
    virtual void LookAtTarget() final;
    virtual void SetDirection( Direction direction ) final;
};