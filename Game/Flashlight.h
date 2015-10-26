#pragma once

#include "Game.h"
#include "SaveFile.h"
#include "Item.h"

class Flashlight {
private:
    ruSceneNode mLight;
    ruSceneNode mModel;

    float mMaxCharge;
    float mCharge;
    float mOnRange;

    ruVector3 mOffset;

    float mRealRange;
    float mRangeDest;
    float mChargeWorkTimeSeconds;
    float mShakeCoeff;

    ruSound mOnSound;
    ruSound mOffSound;
	ruSound mFireSound;
    ruSound mOutOfChargeSound;

	ruSceneNode mFire;
    ruVector3 mInitialPosition;

    ruVector3 mPosition;
    ruVector3 mDestPosition;

	ruAnimation mOpenAnim;
	ruAnimation mCloseAnim;
	ruAnimation mIdleAnim;

	
	static void SerializeAnimation( SaveFile & out, ruAnimation & anim );
	static void DeserializeAnimation( SaveFile & in, ruAnimation & anim );


    bool mOn;

	void Close();
	void Fire();
	void Hide();
	void Show();
	void Open();
public:
	ruEvent OnSwitchOff;
    explicit Flashlight( );
    virtual ~Flashlight();;
    bool GotCharge();
    void Fuel();
    void Attach( ruSceneNode node );
    void SwitchOff();
    void SwitchOn();
    void Switch();
    void Update();
    bool IsOn() const;
    float GetCharge();
	ruSceneNode GetLight();
    bool IsBeamContainsPoint( ruVector3 point ) const;
    Item * CreateAppropriateItem();
    virtual void Serialize( SaveFile & out ) final;
    virtual void Deserialize( SaveFile & in ) final;
};