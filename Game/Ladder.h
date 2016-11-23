#pragma once

#include "Game.h"
#include "SaveFile.h"

class Ladder {
public:
	enum class Status {
		Free,
		ActorStand,
		ActorClimbInTop,
		ActorClimbInBottom,
		ActorClimbingUp,
		ActorClimbingDown,
		ActorClimbOffTop,
		ActorClimbOffBottom
	};
	shared_ptr<ISceneNode> mBegin;
	shared_ptr<ISceneNode> mEnd;
	shared_ptr<ISceneNode> mEnterZone;
	shared_ptr<ISceneNode> mBeginLeavePoint;
	shared_ptr<ISceneNode> mEndLeavePoint;
	Status mStatus;
public:
	Ladder(const shared_ptr<ISceneNode> & hBegin, const shared_ptr<ISceneNode> & hEnd, const shared_ptr<ISceneNode> & hEnterZone, const shared_ptr<ISceneNode> & hBeginLeavePoint, const shared_ptr<ISceneNode> & hEndLeavePoint);
	virtual ~Ladder();
	void Serialize(SaveFile & out);
};