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

    float mMaxAngle;
    float mOffsetAngle;
    float mCurrentAngle;

    State mState;

    ruSoundHandle mOpenSound;
    ruSoundHandle mCloseSound;
public:
    static vector< Door* > msDoorList;

    explicit Door( ruNodeHandle hDoor, float fMaxAngle );
    virtual ~Door();
    bool IsPickedByPlayer();
    void SwitchState();
    State GetState();
    void Open( );
    void Close();
    void DoInteraction();
};