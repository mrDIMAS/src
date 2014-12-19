#pragma once

#include "Game.h"

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
    ruNodeHandle mDoorNode;

    float mMaxAngle;
    float mOffsetAngle;
    float mCurrentAngle;
	TurnDirection mTurnDirection;

    State mState;

	bool mLocked;

    ruSoundHandle mOpenSound;
    ruSoundHandle mCloseSound;
public:
    static vector< Door* > msDoorList;

    explicit Door( ruNodeHandle hDoor, float fMaxAngle );
    virtual ~Door();
    bool IsPickedByPlayer();
    void SwitchState();
    State GetState();
	void SetTurnDirection( TurnDirection direction ) {
		mTurnDirection = direction;
	}
	bool IsLocked()
	{
		return mLocked;
	}
	void SetLocked( bool state ) {
		mLocked = state;
	}
    void Open( );
    void Close();
    virtual void DoInteraction();
};