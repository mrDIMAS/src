#include "Precompiled.h"
#include "Level.h"
#include "Door.h"

Door::~Door() {

}

Door::Door(const shared_ptr<ISceneNode> & hDoor, float fMaxAngle, bool closeSoundOnClosed, const string & openSound, const string & closeSound) :
	mDoorNode(hDoor), mMaxAngle(fMaxAngle), mLocked(false), mCurrentAngle(0.0f), mState(State::Closed) {
	auto soundSystem = hDoor->GetFactory()->GetEngineInterface()->GetSoundSystem();
	mOffsetAngle = hDoor->GetEulerAngles().y;
	SetTurnDirection(TurnDirection::Clockwise);
	mOpenSound = soundSystem->LoadSound3D(openSound);
	mOpenSound->Attach(mDoorNode);
	mOpenSound->SetRolloffFactor(5);
	mOpenSound->SetRoomRolloffFactor(5);
	mOpenSound->SetReferenceDistance(2.5);
	mOpenSound->SetVolume(0.5f);

	mCloseSoundOnClosed = closeSoundOnClosed;

	mCloseSound = soundSystem->LoadSound3D(closeSound);
	mCloseSound->Attach(mDoorNode);
	mCloseSound->SetRolloffFactor(5);
	mCloseSound->SetRoomRolloffFactor(5);
	mCloseSound->SetReferenceDistance(2.5);
	mOpenSound->SetVolume(0.5f);
}

void Door::Update() {
	float turnSpeed = 1.0f;


	if(mState == State::Closing) {
		if(mTurnDirection == TurnDirection::Clockwise) {
			mCurrentAngle -= turnSpeed;
			if(mCurrentAngle < 0) {
				mCurrentAngle = 0.0f;
				mState = State::Closed;

				if(mCloseSoundOnClosed) mCloseSound->Play();
			}
		} else if(mTurnDirection == TurnDirection::Counterclockwise) {
			mCurrentAngle += turnSpeed;
			if(mCurrentAngle > 0) {
				mCurrentAngle = 0;
				mState = State::Closed;

				if(mCloseSoundOnClosed) mCloseSound->Play();
			}
		}

	}

	if(mState == State::Opening) {
		if(mTurnDirection == TurnDirection::Clockwise) {
			mCurrentAngle += turnSpeed;
			if(mCurrentAngle > mMaxAngle) {
				mState = State::Opened;
				mCurrentAngle = mMaxAngle;
			}
		} else if(mTurnDirection == TurnDirection::Counterclockwise) {
			mCurrentAngle -= turnSpeed;
			if(mCurrentAngle < -mMaxAngle) {
				mState = State::Opened;
				mCurrentAngle = -mMaxAngle;
			}
		}

	}

	mDoorNode->SetRotation(Quaternion(Vector3(0, 1, 0), mCurrentAngle + mOffsetAngle));
}

Door::State Door::GetState() {
	return mState;
}

void Door::SwitchState() {
	if(mState == State::Closed) {
		mState = State::Opening;
		mOpenSound->Play();
	}
	if(mState == State::Opened) {
		mState = State::Closing;
		if(!mCloseSoundOnClosed) mCloseSound->Play();
	}
}

void Door::Close() {
	if(mState == State::Opened) {
		mState = State::Closing;
		if(!mCloseSoundOnClosed) mCloseSound->Play();
	}
}

void Door::Open() {
	if(mState == State::Closed) {
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
