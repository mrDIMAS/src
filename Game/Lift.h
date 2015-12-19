#pragma once

#include "Game.h"
#include "LiftDoor.h"
#include "SaveFile.h"

class Lift {
private:
    shared_ptr<ruSceneNode> mBaseNode;
    shared_ptr<ruSceneNode> mControlPanel;
    shared_ptr<ruSceneNode> mSourceNode;
    shared_ptr<ruSceneNode> mDestNode;
    shared_ptr<ruSceneNode> mTargetNode;
    shared_ptr<ruSceneNode> mDoorFrontLeftNode;
    shared_ptr<ruSceneNode> mDoorFrontRightNode;
    shared_ptr<ruSceneNode> mDoorBackLeftNode;
    shared_ptr<ruSceneNode> mDoorBackRightNode;
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
    explicit Lift( shared_ptr<ruSceneNode> base );
    virtual ~Lift( );
    void SetControlPanel( shared_ptr<ruSceneNode> panel );
    void SetDestinationPoint( shared_ptr<ruSceneNode> destNode );
    void SetSourcePoint( shared_ptr<ruSceneNode> sourceNode );
    void SetMotorSound( ruSound motorSound );
    void SetFrontDoors( shared_ptr<ruSceneNode> leftDoor, shared_ptr<ruSceneNode> rightDoor );
    void SetBackDoors( shared_ptr<ruSceneNode> leftDoor, shared_ptr<ruSceneNode> rightDoor );
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