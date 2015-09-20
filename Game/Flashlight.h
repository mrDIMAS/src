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

	

    bool mOn;

	void Close() {
		mFire.Hide();
		mOn = false;
		mOffSound.Play();
	}
	void Fire() {
		mFireSound.Play();
		mFire.Show();
		mOn = true;
	}
	void Hide() {		
		mModel.Hide();
		OnSwitchOff.DoActions();
	}
	void Show() {
		mModel.Show();
	}
	void Open() {		
		mOnSound.Play();
	}
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
	ruSceneNode GetLight() {
		return mLight;
	}
    bool IsBeamContainsPoint( ruVector3 point ) const;
    Item * CreateAppropriateItem();
    virtual void Serialize( SaveFile & out ) final;
    virtual void Deserialize( SaveFile & in ) final;
};