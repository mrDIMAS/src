#pragma once

#include "Game.h"
#include "SaveFile.h"

class Way {
protected:
    shared_ptr<ruSceneNode> mBegin;
    shared_ptr<ruSceneNode> mEnd;
    shared_ptr<ruSceneNode> mEnterZone;
    shared_ptr<ruSceneNode> mTarget;
    shared_ptr<ruSceneNode> mBeginLeavePoint;
    shared_ptr<ruSceneNode> mEndLeavePoint;
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

    Way( shared_ptr<ruSceneNode> hBegin, shared_ptr<ruSceneNode> hEnd, shared_ptr<ruSceneNode> hEnterZone, shared_ptr<ruSceneNode> hBeginLeavePoint, shared_ptr<ruSceneNode> hEndLeavePoint );
    virtual ~Way();
    bool IsEntering();
    bool IsPlayerInside();
    bool IsEnterPicked();
    shared_ptr<ruSceneNode> GetEnterZone();
    shared_ptr<ruSceneNode> GetTarget();
    bool IsFreeLook();
    void DoEntering();
    virtual void Enter();
    virtual void DoPlayerCrawling() = 0;
    virtual void SetDirection( Direction direction ) = 0;
    static Way * GetByObject( shared_ptr<ruSceneNode> obj );
    void Serialize( SaveFile & out );
    void Deserialize( SaveFile & in );
	void LeaveInstantly() {
		mInside = false;
		mLeave = false;
		mEntering = false;
	}
};