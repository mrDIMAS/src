#include "Precompiled.h"
#include "Level.h"
#include "Lift.h"
#include "Player.h"
#include "Utils.h"

Lift::Lift(shared_ptr<ISceneNode> base) :
	mPaused(false),
	mBaseNode(base),
	mArrived(true),
	mEngineSoundEnabled(true),
	mSpeedMultiplier(1.0f),
	mLocked(false) {
	mMotorSound = base->GetFactory()->GetEngineInterface()->GetSoundSystem()->LoadSound3D("data/sounds/elevator_loop.ogg");
	mMotorSound->Attach(mBaseNode);
	mMotorSound->SetRolloffFactor(10);
	mMotorSound->SetRoomRolloffFactor(10);
	mMotorSound->SetReferenceDistance(20);
}

void Lift::Update() {
	if(mBaseNode && mSourceNode && mDestNode && mTargetNode) {

		Vector3 directionVector = mTargetNode->GetPosition() - mBaseNode->GetPosition();
		Vector3 speedVector = directionVector.Normalized() * 0.02f;
		float distSqr = directionVector.Length2();

		if(distSqr < 0.025f) {
			SetDoorsLocked(false);
			mArrived = true;
		}

		if(distSqr > 1.0f) {
			distSqr = 1.0f;
		}


		// smooth arriving
		if(!mArrived) {
			float speed = 1.0f;

			if(distSqr > 1.0f) {
				speed = 1.0f;
			} else {
				speed = distSqr;
			}

			speed *= mSpeedMultiplier;

			if(!mPaused) {
				mBaseNode->Move(speedVector * speed);
			}
		} else {
			mMotorSound->Stop();
		}
	} else {
		throw std::runtime_error("Lift objects are set improperly!");
	}
}

void Lift::SetBackDoors(const shared_ptr<Door> & leftDoor, const shared_ptr<Door> & rightDoor) {
	mDoorBackLeft = leftDoor;
	mDoorBackLeft->SetTurnDirection(Door::TurnDirection::Clockwise);
	mDoorBackRight = rightDoor;
	mDoorBackRight->SetTurnDirection(Door::TurnDirection::Counterclockwise);
}

void Lift::SetFrontDoors(const shared_ptr<Door> & leftDoor, const shared_ptr<Door> & rightDoor) {
	mDoorFrontLeft = leftDoor;
	mDoorFrontLeft->SetTurnDirection(Door::TurnDirection::Clockwise);
	mDoorFrontRight = rightDoor;
	mDoorFrontRight->SetTurnDirection(Door::TurnDirection::Counterclockwise);
}

void Lift::SetSourcePoint(shared_ptr<ISceneNode> sourceNode) {
	mSourceNode = sourceNode;
	mTargetNode = mSourceNode;
}

void Lift::SetDestinationPoint(shared_ptr<ISceneNode> destNode) {
	mDestNode = destNode;
}

Lift::~Lift() {

}

bool Lift::IsArrived() {
	return mArrived;
}

bool Lift::IsAllDoorsClosed() {
	return mDoorFrontLeft->GetState() == Door::State::Closed && mDoorFrontRight->GetState() == Door::State::Closed &&
		mDoorBackLeft->GetState() == Door::State::Closed && mDoorBackRight->GetState() == Door::State::Closed;
}

void Lift::SetDoorsLocked(bool state) {
	mDoorFrontLeft->SetLocked(state);
	mDoorFrontRight->SetLocked(state);
	mDoorBackLeft->SetLocked(state);
	mDoorBackRight->SetLocked(state);
}

void Lift::SetPaused(bool state) {
	mPaused = state;
}

void Lift::SetEngineSoundEnabled(bool state) {
	mEngineSoundEnabled = state;
}

void Lift::SetSpeedMultiplier(float mult) {
	mSpeedMultiplier = mult;
}

void Lift::Serialize(SaveFile & s) {
	s & mArrived;
	s & mPaused;
	s & mEngineSoundEnabled;
	s & mSpeedMultiplier;

	int target = mTargetNode == mSourceNode ? 0 : 1;
	s & target;
	if(target == 0) {
		mTargetNode = mSourceNode;
	} else {
		mTargetNode = mDestNode;
	}
}

