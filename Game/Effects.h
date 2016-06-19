#pragma once

class Effect {
public:
	virtual ~Effect() {

	}

	virtual void Update() = 0;
};

class SteamStream : public Effect {
private:
	friend class EffectFactory;

	shared_ptr<ruParticleSystem > mSteam;
	shared_ptr<ruSound> mSound;
	float mPower;

	SteamStream(const shared_ptr<ruSceneNode> & obj, float length, float thickness) {

		mSteam = ruParticleSystem::Create(60);
		mSteam->SetPosition(obj->GetPosition());
		mSteam->SetType(ruParticleSystem::Type::Stream);
		mSteam->SetSpeedDeviation(ruVector3(-thickness, length * 0.2f, -thickness), ruVector3(thickness, length, thickness));
		mSteam->SetTexture(ruTexture::Request("data/textures/particles/p1.png"));
		mSteam->SetColorRange(ruVector3(255, 255, 255), ruVector3(255, 255, 255));
		mSteam->SetPointSize(0.5f);
		mSteam->SetParticleThickness(1.5f);
		mSteam->SetBoundingRadius(0.8f);
		mSteam->SetLightingEnabled(true);
		mSteam->SetScaleFactor(0.05f);

		mSound = ruSound::Load3D("data/sounds/steamhiss_loop.ogg");
		mSound->Attach(mSteam);
		mSound->SetRolloffFactor(25);
		mSound->SetReferenceDistance(0.4);
		mSound->SetRoomRolloffFactor(25);

		mPower = 1.0f;
	}

	virtual void Update() {
		mSteam->SetPointSize(mPower * 0.15);
		mSteam->SetBoundingRadius(mPower * 0.8);
		mSound->Play();
		mSound->SetVolume(mPower);
	}
public:
	virtual ~SteamStream() {

	}

	void SetPower(float power) {
		mPower = power;
	}
};

class EffectFactory {
private:
	static vector<weak_ptr<Effect>> msEffects;
public:
	static void Update() {
		for (auto weakEffect = msEffects.begin(); weakEffect != msEffects.end(); ) {
			if (weakEffect->expired()) {
				weakEffect = msEffects.erase(weakEffect);
			} else {
				auto effect = weakEffect->lock();

				effect->Update();
			}
		}
	}
};