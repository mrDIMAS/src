#pragma once

#include "SaveFile.h"

class ActionSeries {
private:
	class TimePoint {
	public:
		ruEvent mActions;
		bool mDone;
		TimePoint();
		void Do();
	};
	float mCounter;
	float mLastActionTime;
	bool mEnabled;
	unordered_map<float, TimePoint*> mActions;
public:
	explicit ActionSeries();
	~ActionSeries();
	void IdleActionDummy();
	void SetEnabled(bool state);
	void Rewind();
	void AddAction(float length, const ruDelegate & action);
	void AddSpace(float length);
	void Perform();
	void Deserialize(SaveFile & in);
	void Serialize(SaveFile & out);
};