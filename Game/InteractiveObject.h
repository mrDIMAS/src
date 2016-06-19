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
public:
    shared_ptr<ruSceneNode> mObject;
	ruEvent OnInteract;
    explicit InteractiveObject( shared_ptr<ruSceneNode> object );
	explicit InteractiveObject();
    virtual ~InteractiveObject();
    void UpdateFlashing();
    virtual void Update();
	void SetPickDescription( const string & pd );
	string GetPickDescription() const;
};