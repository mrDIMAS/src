#pragma once

#include "Game.h"


class InteractiveObject {
public:
    NodeHandle node;
    float flashAlbedo;
    float flashAlbedoTo;
    float flashSpeed;
    bool flashDirection;
    static vector< InteractiveObject* > objects;

    InteractiveObject( NodeHandle object );
    ~InteractiveObject();
    void UpdateFlashing();

    virtual void Update() = 0;

    static void UpdateAll();

    static void DeleteAll();
};