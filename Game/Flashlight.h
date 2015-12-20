#pragma once

#include "Game.h"
#include "SaveFile.h"
#include "Item.h"
#include "UsableObject.h"

class Flashlight : public UsableObject {
private:
    shared_ptr<ruPointLight>mLight;

    float mMaxCharge;
    float mCharge;
    float mOnRange;

    ruVector3 mOffset;

    float mRealRange;
    float mRangeDest;
    float mChargeWorkTimeSeconds;
    float mShakeCoeff;

    shared_ptr<ruSound> mOnSound;
    shared_ptr<ruSound> mOffSound;
	shared_ptr<ruSound> mFireSound;

	shared_ptr<ruParticleSystem > mFire;
    ruVector3 mInitialPosition;

    ruVector3 mPosition;
    ruVector3 mDestPosition;

	ruAnimation mOpenAnim;
	ruAnimation mCloseAnim;
	ruAnimation mIdleAnim;
		
	static void SerializeAnimation( SaveFile & out, ruAnimation & anim );
	static void DeserializeAnimation( SaveFile & in, ruAnimation & anim );
	
    bool mOn;

	void Proxy_Close();
	void Proxy_Fire();
	void Proxy_Hide();
	void Proxy_Show();
	void Proxy_Open();

	virtual void OnSerialize( SaveFile & out ) final;
	virtual void OnDeserialize( SaveFile & in ) final;
public:
    explicit Flashlight( );
    ~Flashlight();
    bool GotCharge();
    void Fuel();
    void Attach( shared_ptr<ruSceneNode> node );
    void SwitchOff();
    void SwitchOn();
    bool IsOn() const;
    float GetCharge();
	shared_ptr<ruSceneNode> GetLight();
    bool IsBeamContainsPoint( ruVector3 point );
	virtual void Update() final;
	virtual Item::Type GetItemType( ) final {
		return Item::Type::Lighter;
	}
};