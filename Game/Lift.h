#pragma once

#include "Game.h"
#include "Door.h"
#include "SaveFile.h"
#include "Button.h"

class Lift {
private:
	shared_ptr<ISceneNode> mBaseNode;
	shared_ptr<ISceneNode> mSourceNode;
	shared_ptr<ISceneNode> mDestNode;
	shared_ptr<ISceneNode> mTargetNode;
	shared_ptr<ISound> mMotorSound;
	shared_ptr<Door> mDoorFrontLeft;
	shared_ptr<Door> mDoorFrontRight;
	shared_ptr<Door> mDoorBackLeft;
	shared_ptr<Door> mDoorBackRight;
	bool mArrived;
	bool mPaused;
	bool mEngineSoundEnabled;
	float mSpeedMultiplier;
	bool mLocked;
public:
	explicit Lift(shared_ptr<ISceneNode> base);
	virtual ~Lift();
	void SetDestinationPoint(shared_ptr<ISceneNode> destNode);
	void SetSourcePoint(shared_ptr<ISceneNode> sourceNode);
	void SetFrontDoors(const shared_ptr<Door> & leftDoor, const shared_ptr<Door> & rightDoor);
	void SetBackDoors(const shared_ptr<Door> & leftDoor, const shared_ptr<Door> & rightDoor);
	bool IsArrived();
	void Update();
	void SetDoorsLocked(bool state);
	bool IsAllDoorsClosed();
	void SetPaused(bool state);
	void SetEngineSoundEnabled(bool state);
	void SetSpeedMultiplier(float mult);
	void Serialize(SaveFile & out);
	void SetLocked(bool state)
	{
		mLocked = state;
	}
	bool IsLocked() const
	{
		return mLocked;
	}
	void GoDown()
	{
		if(!mLocked) {
			if(mArrived) {
				if(IsAllDoorsClosed()) {
					SetDoorsLocked(true);
					mTargetNode = mDestNode;
					mMotorSound->Play();
				}
			}
			mArrived = false;
		}
	}
	void GoUp()
	{
		if(!mLocked) {
			if(mArrived) {
				if(IsAllDoorsClosed()) {
					SetDoorsLocked(true);
					mTargetNode = mSourceNode;
					mMotorSound->Play();
				}
			}
			mArrived = false;
		}
	}
};