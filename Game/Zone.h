#pragma once

#include "Player.h"

class Zone {
public:
	enum class Type {
		OneShot,
		Loop,
	};
private:
	ruNodeHandle mObject;
	bool mPlayerInside;
	bool mActive;
	Type mType;
public:
	ruEvent OnPlayerEnter;  // OneShot, Loop
	ruEvent OnPlayerLeave;  // Loop
	ruEvent OnPlayerInside; // Loop

	explicit Zone( ruNodeHandle object );
	void Update();
	void SetActive( bool active );
};