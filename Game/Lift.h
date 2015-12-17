#pragma once

#include "Game.h"
#include "LiftDoor.h"
#include "SaveFile.h"

class Lift {
private:
    ruSceneNode * mBaseNode;
    ruSceneNode * mControlPanel;
    ruSceneNode * mSourceNode;
    ruSceneNode * mDestNode;
    ruSceneNode * mTargetNode;
    ruSceneNode * mDoorFrontLeftNode;
    ruSceneNode * mDoorFrontRightNode;
    ruSceneNode * mDoorBackLeftNode;
    ruSceneNode * mDoorBackRightNode;
    bool mArrived;
	bool mPaused;
	bool mEngineSoundEnabled;
	float mSpeedMultiplier;
    ruSound mMotorSound;
	unique_ptr<LiftDoor> mDoorFrontLeft;
	unique_ptr<LiftDoor> mDoorFrontRight;
	unique_ptr<LiftDoor> mDoorBackLeft;
	unique_ptr<LiftDoor> mDoorBackRight;
public:
    explicit Lift( ruSceneNode * base );
    virtual ~Lift( );
    void SetControlPanel( ruSceneNode * panel );
    void SetDestinationPoint( ruSceneNode * destNode );
    void SetSourcePoint( ruSceneNode * sourceNode );
    void SetMotorSound( ruSound motorSound );
    void SetFrontDoors( ruSceneNode * leftDoor, ruSceneNode * rightDoor );
    void SetBackDoors( ruSceneNode * leftDoor, ruSceneNode * rightDoor );
    bool IsArrived( );
    void Update();
	void SetDoorsLocked( bool state );
	bool IsAllDoorsClosed();
	void SetPaused( bool state );
	void SetEngineSoundEnabled( bool state );
	void SetSpeedMultiplier( float mult );
	void Deserialize( SaveFile & in );
	void Serialize( SaveFile & out );
};