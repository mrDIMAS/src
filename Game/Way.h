#pragma once

#include "Game.h"
#include "SaveFile.h"

class Way {
protected:
    ruSceneNode mBegin;
    ruSceneNode mEnd;
    ruSceneNode mEnterZone;
    ruSceneNode mTarget;
    ruSceneNode mBeginLeavePoint;
    ruSceneNode mEndLeavePoint;
    bool mInside;
    bool mEntering;
    bool mFreeLook;
    bool mLeave;
public:
    static vector<Way*> msWayList;

    enum class Direction {
        Forward  = 1,
        Backward = 2,
    };

    Way( ruSceneNode hBegin, ruSceneNode hEnd, ruSceneNode hEnterZone, ruSceneNode hBeginLeavePoint, ruSceneNode hEndLeavePoint );
    virtual ~Way();
    bool IsEntering();
    bool IsPlayerInside();
    bool IsEnterPicked();
    ruSceneNode GetEnterZone();
    ruSceneNode GetTarget();
    bool IsFreeLook();
    void DoEntering();
    virtual void Enter();
    virtual void DoPlayerCrawling() = 0;
    virtual void SetDirection( Direction direction ) = 0;
    static Way * GetByObject( ruSceneNode obj );
    void Serialize( SaveFile & out );
    void Deserialize( SaveFile & in );
	void LeaveInstantly() {
		mInside = false;
		mLeave = false;
		mEntering = false;
	}
};