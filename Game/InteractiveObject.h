#pragma once

#include "Game.h"

class InteractiveObject {
private:
	float mFlashAlbedo;
	float mFlashAlbedoTo;
	float mFlashSpeed;
	bool mFlashDirection;
public:
    ruSceneNode mObject;
    static vector< InteractiveObject* > msObjectList;
    explicit InteractiveObject( ruSceneNode object );
	explicit InteractiveObject();
    virtual ~InteractiveObject();
    void UpdateFlashing();
    virtual void Update();
    static void UpdateAll();
    static void DeleteAll();
};