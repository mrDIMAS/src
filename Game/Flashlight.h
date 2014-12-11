#pragma once

#include "Game.h"
#include "TextFileStream.h"

class Flashlight
{
public:
    ruNodeHandle light;
    ruNodeHandle model;

    float maxCharge;
    float mCharge;
    float onRange;

    ruVector3 offset;

    float realRange;
    float rangeDest;
    float chargeWorkTimeSeconds;
    float bobArg;

    ruSoundHandle onSound;
    ruSoundHandle offSound;
    ruSoundHandle outOfChargeSound;

    ruVector3 initialPosition;

    ruVector3 position;
    ruVector3 destPosition;

    bool on;

    Flashlight( );
    bool GotCharge();
    void Fuel();
    void Attach( ruNodeHandle node );
    void SwitchOff();
    void SwitchOn();
    void Switch();
    void Update();

    virtual void SerializeWith( TextFileStream & out ) final;
    virtual void DeserializeWith( TextFileStream & in ) final;
};