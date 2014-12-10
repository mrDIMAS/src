#pragma once

#include "Game.h"
#include "TextFileStream.h"

class Way
{
protected:
    ruNodeHandle begin;
    ruNodeHandle end;
    ruNodeHandle enterZone;
    ruNodeHandle target;
    ruNodeHandle beginLeavePoint;
    ruNodeHandle endLeavePoint;
    bool inside;
    bool entering;
    bool freeLook;
    bool leave;
public:
    static vector<Way*> sWayList;

    enum class Direction
    {
        Forward  = 1,
        Backward = 2,
    };

    Way( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint );
    virtual ~Way();
    bool IsEntering();
    bool IsPlayerInside();
    bool IsEnterPicked();
    ruNodeHandle GetEnterZone();
    ruNodeHandle GetTarget();
    bool IsFreeLook();
    void DoEntering();
    virtual void Enter();
    virtual void DoPlayerCrawling() = 0;
    virtual void LookAtTarget() = 0;
    virtual void SetDirection( Direction direction ) = 0;
    static Way * GetByObject( ruNodeHandle obj );
    void SerializeWith( TextFileStream & out );
    void DeserializeWith( TextFileStream & in );
};