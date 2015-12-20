#include "Precompiled.h"

#include "Lamp.h"

void Lamp::Update() {
	PlaySounds();
}

void Lamp::Show() {
	PlaySounds();
}

void Lamp::Hide() {
	mBuzzSound->Pause();
}

Lamp::Lamp( shared_ptr<ruSceneNode> lamp, shared_ptr<ruSound> buzz ) {
	mLamp = lamp;
	mBuzzSound = buzz;
	mBuzzSound->SetRolloffFactor( 2.5f );
	mBuzzSound->SetReferenceDistance( 2.0f );
	mBuzzSound->SetPosition( mLamp->GetPosition() );
}

void Lamp::PlaySounds() {
	if( mBuzzSound ) {
		mBuzzSound->Play();
	}
}
