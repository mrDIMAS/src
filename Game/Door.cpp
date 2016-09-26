#include "Precompiled.h"
#include "Level.h"
#include "Door.h"

Door::~Door() {

}

Door::Door(const shared_ptr<ruSceneNode> & hDoor, float fMaxAngle) : mDoorNode(hDoor), mMaxAngle(fMaxAngle), mLocked(false), mCurrentAngle(0.0f), mState(State::Closed) {
	mOffsetAngle = hDoor->GetEulerAngles().y;
	SetTurnDirection(TurnDirection::Clockwise);
	mOpenSound = ruSound::Load3D("data/sounds/door/dooropen.ogg");
	mOpenSound->Attach(mDoorNode);
	mOpenSound->SetRolloffFactor(20);
	mOpenSound->SetRoomRolloffFactor(20);
	mOpenSound->SetReferenceDistance(0.5);

	mCloseSound = ruSound::Load3D("data/sounds/door/doorclose.ogg");
	mCloseSound->Attach(mDoorNode);
	mCloseSound->SetRolloffFactor(20);
	mCloseSound->SetRoomRolloffFactor(20);
	mCloseSound->SetReferenceDistance(0.5);
}

void Door::Update() {
	float turnSpeed = 1.0f;

	if (mDoorNode->GetContactCount() > 0) {
		turnSpeed = 0.0f;

		if (mState == State::Closing) {
			mCloseSound->Pause();
		}
		if (mState == State::Opening) {
			mOpenSound->Pause();
		}
	} else {
		if (mState == State::Closing) {
			mCloseSound->Play();
		}
		if (mState == State::Opening) {
			mOpenSound->Play();
		}
	}

	if (mState == State::Closing) {
		if (mTurnDirection == TurnDirection::Clockwise) {
			mCurrentAngle -= turnSpeed;
			if (mCurrentAngle < 0) {
				mCurrentAngle = 0.0f;
				mState = State::Closed;
			}
		} else if (mTurnDirection == TurnDirection::Counterclockwise) {
			mCurrentAngle += turnSpeed;
			if (mCurrentAngle > 0) {
				mCurrentAngle = 0;
				mState = State::Closed;
			}
		}

	}

	if (mState == State::Opening) {
		if (mTurnDirection == TurnDirection::Clockwise) {
			mCurrentAngle += turnSpeed;
			if (mCurrentAngle > mMaxAngle) {
				mState = State::Opened;
				mCurrentAngle = mMaxAngle;
			}
		} else if (mTurnDirection == TurnDirection::Counterclockwise) {
			mCurrentAngle -= turnSpeed;
			if (mCurrentAngle < -mMaxAngle) {
				mState = State::Opened;
				mCurrentAngle = -mMaxAngle;
			}
		}

	}

	mDoorNode->SetRotation(ruQuaternion(ruVector3(0, 1, 0), mCurrentAngle + mOffsetAngle));
}

Door::State Door::GetState() {
	return mState;
}

void Door::SwitchState() {
	if (mState == State::Closed) {
		mState = State::Opening;
		mOpenSound->Play();
	}
	if (mState == State::Opened) {
		mState = State::Closing;
		mCloseSound->Play();
	}
}

void Door::Close() {
	if (mState == State::Opened) {
		mState = State::Closing;
		mCloseSound->Play();
	}
}

void Door::Open() {
	if (mState == State::Closed) {
		mState = State::Opening;
		mOpenSound->Play();
	}
}

void Door::Serialize(SaveFile & s) {
	int turnDirection = (int)mTurnDirection;
	int state = (int)mState;

	s & mMaxAngle;
	s & mOffsetAngle;
	s & mCurrentAngle;
	s & turnDirection;
	s & state;
	s & mLocked;

	mTurnDirection = (TurnDirection)turnDirection;
	mState = (State)state;
}

void Door::SetLocked(bool state) {
	mLocked = state;
}

bool Door::IsLocked() {
	return mLocked;
}

void Door::SetTurnDirection(TurnDirection direction) {
	mTurnDirection = direction;
}
