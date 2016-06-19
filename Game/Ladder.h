#pragma once

#include "Game.h"
#include "SaveFile.h"

class Ladder {
protected:
	shared_ptr<ruSceneNode> mBegin;
	shared_ptr<ruSceneNode> mEnd;
	shared_ptr<ruSceneNode> mEnterZone;
	shared_ptr<ruSceneNode> mTarget;
	shared_ptr<ruSceneNode> mBeginLeavePoint;
	shared_ptr<ruSceneNode> mEndLeavePoint;
	bool mInside;
	bool mEntering;
	bool mLeave;
public:
	enum class Direction {
		Forward  = 1,
		Backward = 2,
	};
	Ladder( shared_ptr<ruSceneNode> hBegin, shared_ptr<ruSceneNode> hEnd, shared_ptr<ruSceneNode> hEnterZone, shared_ptr<ruSceneNode> hBeginLeavePoint, shared_ptr<ruSceneNode> hEndLeavePoint );
	virtual ~Ladder();
	bool IsEntering();
	bool IsPlayerInside();
	bool IsEnterPicked();
	shared_ptr<ruSceneNode> GetEnterZone();
	shared_ptr<ruSceneNode> GetTarget();
	void DoEntering();
	void Enter();
	void DoPlayerCrawling();
	void SetDirection( Direction direction );
	void DoLeave();
	void Serialize( SaveFile & out );
	void Deserialize( SaveFile & in );
	void LeaveInstantly();
};