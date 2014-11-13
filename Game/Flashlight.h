#pragma once

#include "Game.h"
#include "TextFileStream.h"

class Flashlight {
public:
    NodeHandle light;
    NodeHandle model;

    float maxCharge;
    float charge;
    float onRange;
	

    float realRange;
    float rangeDest;
	float chargeWorkTimeSeconds;

    SoundHandle onSound;
    SoundHandle offSound;
    SoundHandle outOfChargeSound;

    Vector3 initialPosition;

    Vector3 position;
    Vector3 destPosition;

	Vector3 offset;

    bool on;

    Flashlight( );
    bool GotCharge();
    void Fuel();
    void Attach( NodeHandle node );
    void SwitchOff();
    void SwitchOn();
    void Switch();
    void Update();

    virtual void SerializeWith( TextFileStream & out ) final;
    virtual void DeserializeWith( TextFileStream & in ) final;
};