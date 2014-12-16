#pragma once

#include "Game.h"
#include "TextFileStream.h"
#include "Item.h"

class Flashlight {
private:
    ruNodeHandle mLight;
    ruNodeHandle mModel;

    float mMaxCharge;
    float mCharge;
    float mOnRange;

    ruVector3 mOffset;

    float mRealRange;
    float mRangeDest;
    float mChargeWorkTimeSeconds;
    float mShakeCoeff;

    ruSoundHandle mOnSound;
    ruSoundHandle mOffSound;
    ruSoundHandle mOutOfChargeSound;

    ruVector3 mInitialPosition;

    ruVector3 mPosition;
    ruVector3 mDestPosition;

    bool mOn;
public:
    explicit Flashlight( );
    virtual ~Flashlight();;
    bool GotCharge();
    void Fuel();
    void Attach( ruNodeHandle node );
    void SwitchOff();
    void SwitchOn();
    void Switch();
    void Update();
    bool IsOn() const;
    float GetCharge();
    bool IsBeamContainsPoint( ruVector3 point ) const;
    Item * CreateAppropriateItem();
    virtual void SerializeWith( TextFileStream & out ) final;
    virtual void DeserializeWith( TextFileStream & in ) final;
};