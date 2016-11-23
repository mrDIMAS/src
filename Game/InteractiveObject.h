#pragma once

#include "Game.h"

// Represent fullbright flashing object
class InteractiveObject {
private:
	float mFlashAlbedo;
	float mFlashAlbedoTo;
	float mFlashSpeed;
	bool mFlashDirection;
	int mInteractCountLeft;
	string mPickDesc;
	void UpdateFlashing();
public:
	shared_ptr<ISceneNode> mObject;
	Event OnInteract;
	explicit InteractiveObject(shared_ptr<ISceneNode> object);
	explicit InteractiveObject();
	virtual ~InteractiveObject();
	virtual void Update();
	void SetPickDescription(const string & pd);
	string GetPickDescription() const;
};