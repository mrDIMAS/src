#include "Precompiled.h"
#include "Ladder.h"
#include "Player.h"
#include "Level.h"

Ladder::Ladder(shared_ptr<ruSceneNode> hBegin, shared_ptr<ruSceneNode> hEnd, shared_ptr<ruSceneNode> hEnterZone, shared_ptr<ruSceneNode> hBeginLeavePoint, shared_ptr<ruSceneNode> hEndLeavePoint) {
	mBegin = hBegin;
	mEnd = hEnd;
	mEnterZone = hEnterZone;
	mTarget = hBegin;
	mBeginLeavePoint = hBeginLeavePoint;
	mEndLeavePoint = hEndLeavePoint;

	mEntering = false;
	mInside = false;
	mLeave = false;
}

Ladder::~Ladder() {

}

void Ladder::Enter() {
	mInside = false;
	mEntering = true;
	auto & player = Level::Current()->GetPlayer();
	if ((player->GetCurrentPosition() - mBegin->GetPosition()).Length2() < (player->GetCurrentPosition() - mEnd->GetPosition()).Length2()) {
		mTarget = mBegin;
	}
	else {
		mTarget = mEnd;
	}
	player->Freeze();
}

void Ladder::DoPlayerCrawling() {
	if (!mEntering) {
		auto & player = Level::Current()->GetPlayer();
		ruVector3 direction = mTarget->GetPosition() - player->GetCurrentPosition();
		float distance = direction.Length();
		direction.Normalize();
		if (!mLeave) {
			player->Move(direction, 0.75f);
			if (distance < 0.25f) {
				mLeave = true;
				if ((player->GetCurrentPosition() - mEnd->GetPosition()).Length2() < (player->GetCurrentPosition() - mBegin->GetPosition()).Length2()) {
					mTarget = mEndLeavePoint;
				}
				else {
					mTarget = mBeginLeavePoint;
				}
			}
		}
		else {
			DoLeave();
		}
	}
}

void Ladder::DoEntering() {
	if (mEntering) {
		auto & player = Level::Current()->GetPlayer();
		ruVector3 direction = mTarget->GetPosition() - player->GetCurrentPosition();
		float distance = direction.Length();
		direction.Normalize();
		player->Move(direction, 1.1f);
		if (distance < 0.25f) {
			mEntering = false;
			mInside = true;
			if (mTarget == mEnd) {
				mTarget = mBegin;
			}
			else {
				mTarget = mEnd;
			}
			player->StopInstant();
		}
	}
}

shared_ptr<ruSceneNode> Ladder::GetTarget() {
	return mTarget;
}

bool Ladder::IsEnterPicked() {
	auto & player = Level::Current()->GetPlayer();
	return player->mNearestPickedNode == mEnterZone;
}

bool Ladder::IsPlayerInside() {
	return mInside;
}

bool Ladder::IsEntering() {
	return mEntering;
}

shared_ptr<ruSceneNode> Ladder::GetEnterZone() {
	return mEnterZone;
}

void Ladder::LeaveInstantly()
{
	mInside = false;
	mLeave = false;
	mEntering = false;
}

void Ladder::Serialize(SaveFile & s) {
	s & mInside;
	s & mEntering;
	s & mLeave;

	int targetNum = 0;
	if (mTarget == mBegin) {
		targetNum = 0;
	}
	if (mTarget == mEnd) {
		targetNum = 1;
	}
	if (mTarget == mBeginLeavePoint) {
		targetNum = 2;
	}
	if (mTarget == mEndLeavePoint) {
		targetNum = 3;
	}

	s & targetNum;

	if (s.IsLoading()) {
		if (targetNum == 0) {
			mTarget = mBegin;
		}
		if (targetNum == 1) {
			mTarget = mEnd;
		}
		if (targetNum == 2) {
			mTarget = mBeginLeavePoint;
		}
		if (targetNum == 3) {
			mTarget = mEndLeavePoint;
		}
	}
}

void Ladder::SetDirection(Direction direction)
{
	if (direction == Ladder::Direction::Forward) {
		if (mLeave) {
			mTarget = mEndLeavePoint;
		}
		else {
			mTarget = mEnd;
		}
	}

	if (direction == Ladder::Direction::Backward) {
		if (mLeave) {
			mTarget = mBeginLeavePoint;
		}
		else {
			mTarget = mBegin;
		}
	}
}

void Ladder::DoLeave() {
	auto & player = Level::Current()->GetPlayer();
	ruVector3 direction = mTarget->GetPosition() - player->GetCurrentPosition();
	float distance = direction.Length();
	direction.Normalize();
	player->Move(direction, 0.75f);
	if (distance < 0.25f) {
		mLeave = false;
		mInside = false;
		player->Unfreeze();
		player->StopInstant();
	}
}
