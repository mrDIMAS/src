#pragma once

#include "TextFileStream.h"

class ActionTimer {
private:
	bool mActive;
	float mTimeInterval;
	// due to fixed time step of the main loop, we can use float counter to measure time
	float mTimeCounter;
	// self-delete when elapsed. Warning! In this case do not store pointer to this ActionTimer
	bool mSelfDelete;
	// flag to destructor
	bool mStoragePointerErased;
	// becomes true, when elapsed and mSelfDelete is true 
	bool mCanBeSelfDeleted;
	// static storage of all ActionTimers in game
	static vector<ActionTimer*> msActionTimerList;
	void Update();
public:
	ruEvent OnIdle;
	ruEvent OnBegin;
	ruEvent OnEnd;		
	// don't forget to Activate timer
	explicit ActionTimer( float timeInterval, bool selfDelete );
	~ActionTimer();
	void Activate();
	// useful to control time-critical actions, returns float in interval [0; 1]
	float GetPercentage();
	static void UpdateAll();
	void SerializeWith( TextFileStream & out );
	void DeserializeWith( TextFileStream & in );
};