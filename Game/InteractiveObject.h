#pragma once

#include "Game.h"


class InteractiveObject {
public:
    NodeHandle object;
    float flashAlbedo;
    float flashAlbedoTo;
    float flashSpeed;
    bool flashDirection;
    static vector< InteractiveObject* > objects;

    InteractiveObject( NodeHandle object );
    ~InteractiveObject();
    void UpdateFlashing();

    virtual void Update()
    {
    }

    static void UpdateAll();

    static void DeleteAll();
};