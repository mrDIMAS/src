#include "Precompiled.h"

#include "AmbientSoundSet.h"
#include "Player.h"

void AmbientSoundSet::DoRandomPlaying() {
    if( mTimer->GetElapsedTimeInSeconds() >= mTimeToNextSoundSec ) {
        mTimeToNextSoundSec = frandom( mTimeMinSec, mTimeMaxSec );
        int randomSound = rand() % mSoundList.size();
        mSoundList[ randomSound ]->SetPosition( pPlayer->mpCamera->mCamera->GetPosition() + ruVector3( frandom( -10.0f, 10.0f ), 0.0f, frandom( -10.0f, 10.0f ) ) );
        mSoundList[ randomSound ]->Play();
        mTimer->Restart();
    }
}

void AmbientSoundSet::AddSound( shared_ptr<ruSound> sound ) {
    mSoundList.push_back( sound );
}

void AmbientSoundSet::SetMinMax( float tMin, float tMax ) {
    mTimeMinSec = tMin;
    mTimeMaxSec = tMax;
}

AmbientSoundSet::AmbientSoundSet() {
    mTimeMinSec = 5.0f;
    mTimeMaxSec = 15.0f;
    mTimeToNextSoundSec = mTimeMinSec;
    mTimer = ruTimer::Create();
}

AmbientSoundSet::~AmbientSoundSet() {

}
