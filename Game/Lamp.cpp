#include "Precompiled.h"

#include "Lamp.h"

void Lamp::Update() {
	PlaySounds();
}

void Lamp::Show() {
	PlaySounds();
}

void Lamp::Hide() {
	mBuzzSound.Pause();
}

Lamp::Lamp( ruSceneNode lamp, ruSound buzz ) {
	mLamp = lamp;
	mBuzzSound = buzz;
	mBuzzSound.SetRolloffFactor( 2.5f );
	mBuzzSound.SetReferenceDistance( 2.0f );
	mBuzzSound.SetPosition( mLamp.GetPosition() );
}

void Lamp::PlaySounds() {
	if( mBuzzSound.IsValid() ) {
		mBuzzSound.Play();
	}
}
