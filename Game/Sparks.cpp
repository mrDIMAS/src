#include "Precompiled.h"

#include "Sparks.h"

void Sparks::Update()
{
	if(mParticleSystem->GetAliveParticles() <= 0) {
		mAlive = 0;
		mParticleSystem.reset();
	}
}

Sparks::Sparks(shared_ptr<ISceneNode> at, shared_ptr<ISound> emits)
{
	mParticleSystem = at->GetFactory()->CreateParticleSystem(50);
	mParticleSystem->Attach(at);
	mParticleSystem->SetTexture(at->GetFactory()->GetEngineInterface()->GetRenderer()->GetTexture("data/textures/particles/p1.png"));
	mParticleSystem->SetType(IParticleSystem::Type::Box);
	mParticleSystem->SetSpeedDeviation(Vector3(-0.0015, 0.02, -0.0015), Vector3(0.0015, -0.09, 0.0015));
	mParticleSystem->SetColorRange(Vector3(255, 255, 255), Vector3(255, 255, 0));
	mParticleSystem->SetPointSize(0.025f);
	mParticleSystem->SetBoundingBox(Vector3(-0.08, -0.2, -0.08), Vector3(0.08, 0.1, 0.08));
	mParticleSystem->SetParticleThickness(20.5f);
	mParticleSystem->SetAutoResurrection(false);
	mParticleSystem->SetLightingEnabled(false);

	emits->Attach(mParticleSystem);
	emits->Play();
	mSound = emits;
	mAlive = 1;
}

bool Sparks::IsAlive()
{
	return mAlive;
}

Sparks::~Sparks()
{

}
