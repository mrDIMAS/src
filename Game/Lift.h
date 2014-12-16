#pragma once

#include "Game.h"

class Lift {
private:
    ruNodeHandle mBaseNode;
    ruNodeHandle mControlPanel;
    ruNodeHandle mSourceNode;
    ruNodeHandle mDestNode;
    ruNodeHandle mTargetNode;
    ruNodeHandle mDoorFrontLeft;
    ruNodeHandle mDoorFrontRight;
    ruNodeHandle mDoorBackLeft;
    ruNodeHandle mDoorBackRight;
    bool mArrived;
    ruSoundHandle mMotorSound;
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
};