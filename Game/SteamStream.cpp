#include "Precompiled.h"

#include "SteamStream.h"

void SteamStream::Update() {
    mSteam->SetPointSize( mPower * 0.15 );
    mSteam->SetBoundingRadius( mPower * 0.8 );
    mSound.Play();
    mSound.SetVolume( mPower );
}

SteamStream::SteamStream( shared_ptr<ruSceneNode> obj, ruVector3 speedMin, ruVector3 speedMax, ruSound hissSound ) {
    mSound = hissSound;
	mSteam = ruParticleSystem::Create( 60 );
	mSteam->SetPosition( obj->GetPosition() );
	mSteam->SetType( ruParticleSystem::Type::Stream );
	mSteam->SetSpeedDeviation( speedMin, speedMax );
	mSteam->SetTexture( ruTexture::Request( "data/textures/particles/p1.png" ));
	mSteam->SetColorRange( ruVector3( 255, 255, 255 ), ruVector3( 255, 255, 255 ));
	mSteam->SetPointSize( 0.5f );
	mSteam->SetParticleThickness( 1.5f );
	mSteam->SetBoundingRadius( 0.8f );
	mSteam->SetLightingEnabled( true );
	mSteam->SetScaleFactor( 0.05f );

    mSound.Attach( mSteam );
	mSound.SetRolloffFactor( 25 );
	mSound.SetReferenceDistance( 0.4 );
	mSound.SetRoomRolloffFactor( 25 );
    mPower = 1;
}

SteamStream::~SteamStream() {
	mSound.Free();
}
