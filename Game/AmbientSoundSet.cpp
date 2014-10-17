#include "AmbientSoundSet.h"

#include "Player.h"

void AmbientSoundSet::DoRandomPlaying() {
    if( GetElapsedTimeInSeconds( timer ) >= timeToNextSoundSec ) {
        timeToNextSoundSec = frandom( timeMinSec, timeMaxSec );

        int randomSound = rand() % sounds.size();

        SetSoundPosition( sounds[ randomSound ], GetPosition( player->camera->cameraNode ) + Vector3( frandom( -10.0f, 10.0f ), 0.0f, frandom( -10.0f, 10.0f ) ) );

        PlaySoundSource( sounds[ randomSound ], true );

        RestartTimer( timer );
    }
}

void AmbientSoundSet::AddSound( SoundHandle sound ) {
    sounds.push_back( sound );
}

void AmbientSoundSet::SetMinMax( float tMin, float tMax ) {
    timeMinSec = tMin;
    timeMaxSec = tMax;
}

AmbientSoundSet::AmbientSoundSet() {
    timeMinSec = 5.0f;
    timeMaxSec = 15.0f;

    timeToNextSoundSec = timeMinSec;

    timer = CreateTimer();
}
