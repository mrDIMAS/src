#include "Precompiled.h"

#include "AmbientSoundSet.h"
#include "Player.h"

void AmbientSoundSet::DoRandomPlaying() {
    if( ruGetElapsedTimeInSeconds( timer ) >= timeToNextSoundSec ) {
        timeToNextSoundSec = frandom( timeMinSec, timeMaxSec );

        int randomSound = rand() % sounds.size();

        ruSetSoundPosition( sounds[ randomSound ], ruGetNodePosition( pPlayer->mpCamera->mNode ) + ruVector3( frandom( -10.0f, 10.0f ), 0.0f, frandom( -10.0f, 10.0f ) ) );

        ruPlaySound( sounds[ randomSound ], true );

        ruRestartTimer( timer );
    }
}

void AmbientSoundSet::AddSound( ruSoundHandle sound ) {
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

    timer = ruCreateTimer();
}
