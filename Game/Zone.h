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

	Zone(const shared_ptr<ruSceneNode> & object, const ruDelegate & onEnter) :
		mObject(object),
		mType(Type::OneShot),
		mPlayerInside(false),
		mActive(true)
	{
		OnPlayerEnter += onEnter;
	}

	void Update();
	void SetActive(bool active);
};