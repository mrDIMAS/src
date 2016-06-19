#pragma once

#include "Game.h"
#include "SaveFile.h"

class Door {
protected:
	friend class Level;
	explicit Door(const shared_ptr<ruSceneNode> & hDoor, float fMaxAngle);
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
	shared_ptr<ruSceneNode> mDoorNode;
	float mMaxAngle;
	float mOffsetAngle;
	float mCurrentAngle;
	TurnDirection mTurnDirection;
	State mState;
	bool mLocked;
	shared_ptr<ruSound> mOpenSound;
	shared_ptr<ruSound> mCloseSound;
public:
	virtual ~Door();
	void SwitchState();
	State GetState();
	float SetOffsetAngle(float angle) {
		mOffsetAngle = angle;
	}
	shared_ptr<ruSceneNode> GetNode() {
		return mDoorNode;
	}
	void SetTurnDirection(TurnDirection direction);
	bool IsLocked();
	void SetLocked(bool state);
	void Open();
	void Close();
	virtual void Update();
	void Serialize(SaveFile & out);
	void Deserialize(SaveFile & in);
};