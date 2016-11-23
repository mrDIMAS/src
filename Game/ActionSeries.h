#pragma once

#include "SaveFile.h"

class ActionSeries {
private:
	class TimePoint {
	public:
		Event mActions;
		bool mDone;
		TimePoint();
		void Do();
	};
	float mCounter;
	float mLastActionTime;
	bool mEnabled;
	unordered_map<float, unique_ptr<TimePoint>> mActions;
public:
	explicit ActionSeries();
	~ActionSeries();
	void IdleActionDummy();
	void SetEnabled(bool state);
	void Rewind();
	void AddAction(float length, const Delegate & action);
	void AddSpace(float length);
	void Perform();
	void Serialize(SaveFile & out);
};