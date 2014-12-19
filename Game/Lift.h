#pragma once

#include "Game.h"
#include "LiftDoor.h"
class Lift {
private:
    ruNodeHandle mBaseNode;
    ruNodeHandle mControlPanel;
    ruNodeHandle mSourceNode;
    ruNodeHandle mDestNode;
    ruNodeHandle mTargetNode;
    ruNodeHandle mDoorFrontLeftNode;
    ruNodeHandle mDoorFrontRightNode;
    ruNodeHandle mDoorBackLeftNode;
    ruNodeHandle mDoorBackRightNode;
    bool mArrived;
    ruSoundHandle mMotorSound;
	LiftDoor * mDoorFrontLeft;
	LiftDoor * mDoorFrontRight;
	LiftDoor * mDoorBackLeft;
	LiftDoor * mDoorBackRight;
public:
    explicit Lift( ruNodeHandle base );
    virtual ~Lift( );
    void SetControlPanel( ruNodeHandle panel );
    void SetDestinationPoint( ruNodeHandle destNode );
    void SetSourcePoint( ruNodeHandle sourceNode );
    void SetMotorSound( ruSoundHandle motorSound );
    void SetFrontDoors( ruNodeHandle leftDoor, ruNodeHandle rightDoor );
    void SetBackDoors( ruNodeHandle leftDoor, ruNodeHandle rightDoor );
    bool IsArrived( );
    void Update();
	void SetDoorsLocked( bool state );
	bool IsAllDoorsClosed();
};