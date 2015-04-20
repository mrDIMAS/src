#include "Precompiled.h"

#include "Lamp.h"

void Lamp::Update()
{
	PlaySounds();
}

void Lamp::Show()
{
	PlaySounds();
}

void Lamp::Hide()
{
	ruPauseSound( mBuzzSound );
}

Lamp::Lamp( ruNodeHandle lamp, ruSoundHandle buzz )
{
	mLamp = lamp;
	mBuzzSound = buzz;
	ruSetRolloffFactor( mBuzzSound, 5 );
	ruSetRolloffFactor( mBuzzSound, 2.5f );
	ruSetSoundReferenceDistance( mBuzzSound, 2.0f );
	ruSetSoundPosition( mBuzzSound, ruGetNodePosition( mLamp ));
}

void Lamp::PlaySounds()
{
	if( mBuzzSound.IsValid() ) {
		ruPlaySound( mBuzzSound );
	}
}
