#include "Precompiled.h"
#include "ActionSeries.h"

extern double gFixedTick;

ActionSeries::ActionSeries() : mLastActionTime(0.0f), mCounter(0.0f), mEnabled(false) {

}

void ActionSeries::Perform() {
	if (mEnabled) {
		mCounter += 1.0f / 60.0f;
		for (auto timeEventPair : mActions) {
			if (!timeEventPair.second->mDone) {
				if (mCounter > timeEventPair.first) {
					timeEventPair.second->Do();
				}
			}
		}
	}
}

void ActionSeries::AddSpace(float length) {
	AddAction(length, ruDelegate::Bind(this, &ActionSeries::IdleActionDummy));
}

void ActionSeries::AddAction(float length, const ruDelegate & action) {
	mLastActionTime += length;
	mActions[mLastActionTime] = new TimePoint();
	mActions[mLastActionTime]->mActions.AddListener(action);
	mActions[mLastActionTime]->mDone = false;
}

void ActionSeries::Rewind() {
	mCounter = 0.0f;
}

void ActionSeries::SetEnabled(bool state) {
	mEnabled = state;
}

void ActionSeries::IdleActionDummy() {
	// dummy
}

ActionSeries::~ActionSeries() {
	for (auto timePointPair : mActions) {
		delete timePointPair.second;
	}
}

void ActionSeries::Serialize(SaveFile & s) {
	s & mEnabled;
	s & mCounter;
	s & mLastActionTime;
	for (auto timePointPair : mActions) {
		s & timePointPair.second->mDone;
		if (s.IsLoading()) {
			if (timePointPair.second->mDone) {
				timePointPair.second->Do();
			}
		}
	}
}

void ActionSeries::TimePoint::Do() {
	mActions.DoActions();
	mDone = true;
}

ActionSeries::TimePoint::TimePoint() : mDone(false) {

}
