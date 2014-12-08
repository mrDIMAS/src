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
    ruNodeHandle door;

    float maxAngle;
    float offsetAngle;
    float currentAngle;

    State state;

    ruSoundHandle openSound;
    ruSoundHandle closeSound;
public:

    static vector< Door* > all;

    explicit Door( ruNodeHandle hDoor, float fMaxAngle );
    virtual ~Door();
    bool IsPickedByPlayer();
    void SwitchState();
    State GetState();
	void Open( );
	void Close();
    void DoInteraction();
};