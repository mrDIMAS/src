#include "Precompiled.h"

#include "Sparks.h"

void Sparks::Update() {
    if( mParticleSystem->GetAliveParticles() <= 0 ) {
        mAlive = 0;
        mParticleSystem.reset();
        mSound.Free();
    }
}

Sparks::Sparks( shared_ptr<ruSceneNode> at, ruSound emits ) {
	mParticleSystem = ruParticleSystem::Create( 50 );
	mParticleSystem->Attach( at );
	mParticleSystem->SetTexture( ruTexture::Request( "data/textures/particles/p1.png") );
	mParticleSystem->SetType( ruParticleSystem::Type::Box );
	mParticleSystem->SetSpeedDeviation( ruVector3( -0.0015, 0.02, -0.0015 ), ruVector3( 0.0015, -0.09, 0.0015 ));
	mParticleSystem->SetColorRange( ruVector3( 255, 255, 255 ), ruVector3( 255, 255, 0 ));
	mParticleSystem->SetPointSize( 0.025f );
	mParticleSystem->SetBoundingBox( ruVector3( -0.08, -0.2, -0.08 ), ruVector3( 0.08, 0.1, 0.08 ));
	mParticleSystem->SetParticleThickness( 20.5f );
	mParticleSystem->SetAutoResurrection( false );
	mParticleSystem->SetLightingEnabled( false );

    emits.Attach( mParticleSystem );
    emits.Play();
    mSound = emits;
    mAlive = 1;
}

bool Sparks::IsAlive()
{
	return mAlive;
}

Sparks::~Sparks()
{
	mSound.Free();
}
