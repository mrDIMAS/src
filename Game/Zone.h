#pragma once

#include "Player.h"

class Zone {
public:
	enum class Type {
		OneShot,
		Loop,
	};
private:
	shared_ptr<ruSceneNode> mObject;
	bool mPlayerInside;
	bool mActive;
	Type mType;
public:
	ruEvent OnPlayerEnter;  // OneShot, Loop
	ruEvent OnPlayerLeave;  // Loop
	ruEvent OnPlayerInside; // Loop

	explicit Zone( shared_ptr<ruSceneNode> object );
	void Update();
	void SetActive( bool active );
};