#pragma once

#include "Game.h"


class InteractiveObject {
private:
	float mFlashAlbedo;
	float mFlashAlbedoTo;
	float mFlashSpeed;
	bool mFlashDirection;
public:
    ruNodeHandle mObject;

    static vector< InteractiveObject* > objects;

    explicit InteractiveObject( ruNodeHandle object );
    virtual ~InteractiveObject();
    void UpdateFlashing();

    virtual void Update() {
    }

    static void UpdateAll();

    static void DeleteAll();
};