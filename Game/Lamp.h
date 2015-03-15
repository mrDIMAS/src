#pragma once

#include "Game.h"

class Lamp {
private:
	void PlaySounds() {
		if( mBuzzSound.IsValid() ) {
			ruPlaySound( mBuzzSound );
		}
	}
public:
	ruSoundHandle mBuzzSound;
	ruNodeHandle mLamp;

	explicit Lamp( ruNodeHandle lamp, ruSoundHandle buzz ) {
		mLamp = lamp;
		mBuzzSound = buzz;
		ruSetRolloffFactor( mBuzzSound, 5 );
		ruSetRolloffFactor( mBuzzSound, 2.5f );
		ruSetSoundReferenceDistance( mBuzzSound, 2.0f );
		ruSetSoundPosition( mBuzzSound, ruGetNodePosition( mLamp ));
	}

	void Hide() {
		ruPauseSound( mBuzzSound );
	}

	void Show( ) {
		PlaySounds();
	}

	void Update() {
		PlaySounds();
	}
};