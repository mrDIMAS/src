#pragma once

#include "Way.h"


class CrawlWay : public Way
{
private:
  void DoLeave();
public:
 
  explicit CrawlWay( NodeHandle hBegin, NodeHandle hEnd, NodeHandle hEnterZone, NodeHandle hBeginLeavePoint, NodeHandle hEndLeavePoint );
  virtual ~CrawlWay(); 
  virtual void DoPlayerCrawling() final;
  virtual void LookAtTarget() final;
  virtual void SetDirection( Direction direction ) final;
};