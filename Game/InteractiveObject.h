#pragma once

#include "Game.h"


class InteractiveObject
{
public:
    ruNodeHandle mObject;
    float flashAlbedo;
    float flashAlbedoTo;
    float flashSpeed;
    bool flashDirection;
    static vector< InteractiveObject* > objects;

    InteractiveObject( ruNodeHandle object );
    ~InteractiveObject();
    void UpdateFlashing();

    virtual void Update()
    {
    }

    static void UpdateAll();

    static void DeleteAll();
};