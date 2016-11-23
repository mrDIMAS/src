#pragma once

#include "Game.h"
#include "SaveFile.h"

class Door {
public:
	enum class State {
		Opened,
		Closed,
		Closing,
		Opening,
	};
	enum class TurnDirection {
		Clockwise,
		Counterclockwise,
	};
public:
	shared_ptr<ISceneNode> mDoorNode;
	float mMaxAngle;
	float mOffsetAngle;
	float mCurrentAngle;
	TurnDirection mTurnDirection;
	State mState;
	bool mLocked;
	shared_ptr<ISound> mOpenSound;
	shared_ptr<ISound> mCloseSound;
public:
	explicit Door(const shared_ptr<ISceneNode> & hDoor, float fMaxAngle);
	virtual ~Door();
	void SwitchState();
	State GetState();
	float SetOffsetAngle(float angle)
	{
		mOffsetAngle = angle;
	}
	shared_ptr<ISceneNode> GetNode()
	{
		return mDoorNode;
	}
	void SetTurnDirection(TurnDirection direction);
	bool IsLocked();
	void SetLocked(bool state);
	void Open();
	void Close();
	virtual void Update();
	void Serialize(SaveFile & out);
};