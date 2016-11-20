#include "Precompiled.h"
#include "Ladder.h"
#include "Player.h"
#include "Level.h"

Ladder::Ladder(const shared_ptr<ruSceneNode> & hBegin, const shared_ptr<ruSceneNode> & hEnd, const shared_ptr<ruSceneNode> & hEnterZone, const shared_ptr<ruSceneNode> & hBeginLeavePoint, const shared_ptr<ruSceneNode> & hEndLeavePoint) :
	mBegin(hBegin),
	mEnd(hEnd),
	mEnterZone(hEnterZone),
	mBeginLeavePoint(hBeginLeavePoint),
	mEndLeavePoint(hEndLeavePoint),
	mStatus(Status::Free)
{

}

Ladder::~Ladder()
{

}


void Ladder::Serialize(SaveFile & s)
{
	int status = (int)mStatus;
	s & status;
	mStatus = (Status)status;
}
