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
	shared_ptr<ruSceneNode> mBegin;
	shared_ptr<ruSceneNode> mEnd;
	shared_ptr<ruSceneNode> mEnterZone;
	shared_ptr<ruSceneNode> mBeginLeavePoint;
	shared_ptr<ruSceneNode> mEndLeavePoint;
	Status mStatus;
public:
	Ladder(const shared_ptr<ruSceneNode> & hBegin, const shared_ptr<ruSceneNode> & hEnd, const shared_ptr<ruSceneNode> & hEnterZone, const shared_ptr<ruSceneNode> & hBeginLeavePoint, const shared_ptr<ruSceneNode> & hEndLeavePoint);
	virtual ~Ladder();
	void Serialize(SaveFile & out);
};