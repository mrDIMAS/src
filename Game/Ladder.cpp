#include "Precompiled.h"
#include "Ladder.h"
#include "Player.h"
#include "Level.h"

Ladder::Ladder(const shared_ptr<ISceneNode> & hBegin, const shared_ptr<ISceneNode> & hEnd, const shared_ptr<ISceneNode> & hEnterZone, const shared_ptr<ISceneNode> & hBeginLeavePoint, const shared_ptr<ISceneNode> & hEndLeavePoint) :
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
