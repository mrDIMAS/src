#pragma once

#include "Game.h"

class Door {
public:
    enum class State {
        Opened,
        Closed,
        Closing,
        Opening,
    };
public:
    NodeHandle door;

    float maxAngle;
    float offsetAngle;
    float currentAngle;

    State state;

    SoundHandle openSound;
    SoundHandle closeSound;
public:

    static vector< Door* > all;

    explicit Door( NodeHandle hDoor, float fMaxAngle );
    virtual ~Door();
    bool IsPickedByPlayer();
    void SwitchState();
    State GetState();
	void Open( );
	void Close();
    void DoInteraction();
};