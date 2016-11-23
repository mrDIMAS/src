#pragma once

#include "Player.h"

class Zone {
public:
	enum class Type {
		OneShot,
		Loop,
	};
private:
	shared_ptr<ISceneNode> mObject;
	bool mPlayerInside;
	bool mActive;
	Type mType;
public:
	Event OnPlayerEnter;  // OneShot, Loop
	Event OnPlayerLeave;  // Loop
	Event OnPlayerInside; // Loop

	Zone(const shared_ptr<ISceneNode> & object, const Delegate & onEnter) :
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