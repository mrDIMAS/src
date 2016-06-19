#include "Precompiled.h"
#include "Level.h"
#include "Flashlight.h"
#include "Player.h"

Flashlight::Flashlight() {
	mModel = ruSceneNode::LoadFromFile("data/models/hands/arm_anim.scene");
	mModel->SetDepthHack(0.155f);

	mLight = std::dynamic_pointer_cast<ruPointLight>(mModel->FindChild("PlayerLight"));

	mOnSound = ruSound::Load2D("data/sounds/lighter/open.ogg");
	mOnSound->SetVolume(0.3f);

	mOffSound = ruSound::Load2D("data/sounds/lighter/close.ogg");
	mOffSound->SetVolume(0.3f);

	mFireSound = ruSound::Load2D("data/sounds/lighter/fire.ogg");

	mOnRange = mLight->GetRange() * 1.25f; // HAAAAAAAAAAAAAAAX!

	mRealRange = mOnRange;
	mRangeDest = mOnRange;

	mMaxCharge = 1.0f;
	mCharge = mMaxCharge;

	mChargeWorkTimeSeconds = 240.0f;
	mShakeCoeff = 0.0f;

	mCloseAnim = ruAnimation(20, 34, 0.9);
	mCloseAnim.AddFrameListener(26, ruDelegate::Bind(this, &Flashlight::Proxy_Close));
	mCloseAnim.AddFrameListener(34, ruDelegate::Bind(this, &Flashlight::Proxy_Hide));
	mOpenAnim = ruAnimation(35, 70, 2.8);
	mOpenAnim.AddFrameListener(44, ruDelegate::Bind(this, &Flashlight::Proxy_Open));
	mOpenAnim.AddFrameListener(55, ruDelegate::Bind(this, &Flashlight::Proxy_Fire));
	mOpenAnim.AddFrameListener(35, ruDelegate::Bind(this, &Flashlight::Proxy_Show));
	mIdleAnim = ruAnimation(0, 19, 5);

	mFire = ruParticleSystem::Create(60);
	mFire->Attach(mModel->FindChild("FirePlace"));
	mFire->SetType(ruParticleSystem::Type::Stream);
	mFire->SetSpeedDeviation(ruVector3(-0.00001f, 0.001f, -0.00001f), ruVector3(0.000012f, 0.0015f, 0.000012f));
	mFire->SetTexture(ruTexture::Request("data/textures/particles/p1.png"));
	mFire->SetColorRange(ruVector3(255, 127, 39), ruVector3(0, 55, 244));
	mFire->SetPointSize(0.0095f);
	mFire->SetParticleThickness(1.5f);
	mFire->SetBoundingRadius(0.004f);
	mFire->SetLightingEnabled(false);
	mFire->SetDepthHack(0.1551f);
	mFire->SetScaleFactor(-0.00008f);

	mOn = false;
	mLight->SetRange(0.001f);

	mFire->Hide();
	mModel->Hide();
}

void Flashlight::SwitchOn() {
	if (!mCloseAnim.IsEnabled() && !mOpenAnim.IsEnabled()) {
		if (!mOn) {
			mOpenAnim.Rewind();
			mModel->SetAnimation(&mOpenAnim);
			mOpenAnim.SetEnabled(true);
			mIdleAnim.SetEnabled(false);
		}
	}
}

void Flashlight::SwitchOff() {
	if (!mOpenAnim.IsEnabled() && !mCloseAnim.IsEnabled()) {
		if (mOn) {
			mCloseAnim.Rewind();
			mModel->SetAnimation(&mCloseAnim);
			mCloseAnim.SetEnabled(true);
			mIdleAnim.SetEnabled(false);
		}
	}
}

void Flashlight::Attach(shared_ptr<ruSceneNode> node) {
	mModel->Attach(node);

	mInitialPosition = mModel->GetPosition();
	mDestPosition = mInitialPosition;
	mPosition = mInitialPosition;
}

void Flashlight::Fuel() {
	mCharge = mMaxCharge;
}

bool Flashlight::GotCharge() {
	return mCharge > 0.0f;
}

Flashlight::~Flashlight() {

}

bool Flashlight::IsBeamContainsPoint(ruVector3 point) {
	return mLight->IsSeePoint(point);
}

float Flashlight::GetCharge() {
	return mCharge;
}

bool Flashlight::IsOn() const {
	return mOn;
}

shared_ptr<ruSceneNode> Flashlight::GetLight() {
	return mLight;
}

void Flashlight::Proxy_Open() {
	mOnSound->Play();
}

void Flashlight::Proxy_Show() {
	mModel->Show();
}

void Flashlight::Proxy_Hide() {
	SwitchIfAble();
}

void Flashlight::Proxy_Fire() {
	mFireSound->Play();
	mFire->Show();
	mOn = true;
}

void Flashlight::Proxy_Close() {
	mFire->Hide();
	mOn = false;
	mOffSound->Play();
}

void Flashlight::DeserializeAnimation(SaveFile & in, ruAnimation & anim) {
	anim.SetCurrentFrame(in.ReadInteger());
	anim.SetEnabled(in.ReadBoolean());
}

void Flashlight::SerializeAnimation(SaveFile & out, ruAnimation & anim) {
	out.WriteInteger(anim.GetCurrentFrame());
	out.WriteBoolean(anim.IsEnabled());
}

void Flashlight::OnDeserialize(SaveFile & in) {
	in.ReadFloat(mMaxCharge);
	in.ReadFloat(mCharge);
	in.ReadFloat(mOnRange);
	in.ReadFloat(mRealRange);
	in.ReadFloat(mRangeDest);
	in.ReadBoolean(mOn);
	if (mOn) {
		SwitchOn();
	} else {
		SwitchOff();
	}

	DeserializeAnimation(in, mCloseAnim);
	DeserializeAnimation(in, mIdleAnim);
	DeserializeAnimation(in, mOpenAnim);

	int currentAnim = in.ReadInteger();
	if (currentAnim == 0) {
		mModel->SetAnimation(&mCloseAnim);
	}
	if (currentAnim == 1) {
		mModel->SetAnimation(&mIdleAnim);
	}
	if (currentAnim == 2) {
		mModel->SetAnimation(&mOpenAnim);
	}
}

void Flashlight::OnSerialize(SaveFile & out) {
	out.WriteFloat(mMaxCharge);
	out.WriteFloat(mCharge);
	out.WriteFloat(mOnRange);
	out.WriteFloat(mRealRange);
	out.WriteFloat(mRangeDest);
	out.WriteBoolean(mOn);

	SerializeAnimation(out, mCloseAnim);
	SerializeAnimation(out, mIdleAnim);
	SerializeAnimation(out, mOpenAnim);

	if (mModel->GetCurrentAnimation() == &mCloseAnim) {
		out.WriteInteger(0);
	} else if (mModel->GetCurrentAnimation() == &mIdleAnim) {
		out.WriteInteger(1);
	} else if (mModel->GetCurrentAnimation() == &mOpenAnim) {
		out.WriteInteger(2);
	} else {
		out.WriteInteger(-1);
	}
}

void Flashlight::Update() {
	mLight->SetGreyscaleFactor(0.0f);

	if (mAppear) {
		SwitchOn();
		mAppear = false;
	}

	if (mToNext || mToPrev) {
		SwitchOff();
	}

	if (mOn) {
		mRangeDest = mOnRange;

		mCharge -= 0.01666f / mChargeWorkTimeSeconds;

		if (mCharge < 0.01f) {
			mFire->Hide();
			mCharge = 0.01f;
		} else {
			mFire->Show();
		}
	} else {
		mRangeDest = 0.0f;
	}

	if (mCharge > 0.025f) {
		mRealRange = mRangeDest;
	} else {
		mRealRange += (mRangeDest * mCharge - mRealRange) * 0.025f;
	}
	mLight->SetRange(mRealRange);
	mPosition = mPosition.Lerp(mDestPosition, 0.15f);
	mModel->SetPosition(mPosition + mOffset);

	auto & player = Level::Current()->GetPlayer();
	if (player->mMoved) {
		if (player->mRunning) {
			mShakeCoeff += 0.190f;
		} else {
			mShakeCoeff += 0.125f;
		}
		mOffset = ruVector3(cosf(mShakeCoeff * 0.5f) * 0.005f, sinf(mShakeCoeff) * 0.01f, 0.0f);
	}

	mOpenAnim.Update();
	mCloseAnim.Update();
	mIdleAnim.Update();

	if (mOn && !mCloseAnim.IsEnabled() && !mOpenAnim.IsEnabled()) {
		mModel->SetAnimation(&mIdleAnim);
		mIdleAnim.SetEnabled(true);
	}
}
