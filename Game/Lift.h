#pragma once

#include "Game.h"
#include "Door.h"
#include "SaveFile.h"

class Lift {
private:
	friend class Level;
	explicit Lift( shared_ptr<ruSceneNode> base );

    shared_ptr<ruSceneNode> mBaseNode;
    shared_ptr<ruSceneNode> mControlPanel;
    shared_ptr<ruSceneNode> mSourceNode;
    shared_ptr<ruSceneNode> mDestNode;
    shared_ptr<ruSceneNode> mTargetNode;
    shared_ptr<ruSound> mMotorSound;
	shared_ptr<Door> mDoorFrontLeft;
	shared_ptr<Door> mDoorFrontRight;
	shared_ptr<Door> mDoorBackLeft;
	shared_ptr<Door> mDoorBackRight;
	bool mArrived;
	bool mPaused;
	bool mEngineSoundEnabled;
	float mSpeedMultiplier;
public:

    virtual ~Lift( );
    void SetControlPanel( shared_ptr<ruSceneNode> panel );
    void SetDestinationPoint( shared_ptr<ruSceneNode> destNode );
    void SetSourcePoint( shared_ptr<ruSceneNode> sourceNode );
    void SetFrontDoors( const shared_ptr<Door> & leftDoor, const shared_ptr<Door> & rightDoor );
    void SetBackDoors( const shared_ptr<Door> & leftDoor, const shared_ptr<Door> & rightDoor );
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