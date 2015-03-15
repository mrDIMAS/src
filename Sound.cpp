#include "Common.h"

ruSoundHandle::ruSoundHandle() {
    pfHandle = -1;
}

ruSoundHandle::~ruSoundHandle() {

}

bool ruSoundHandle::IsValid() {
    return pfHandle >= 0;
}

void ruSoundHandle::Invalidate() {
    pfHandle = -1;
}

bool ruSoundHandle::operator == ( const ruSoundHandle & node ) {
    return pfHandle == node.pfHandle;
}

ruSoundHandle ruLoadSound2D( const string & file ) {
    ruSoundHandle handle;
    handle.pfHandle = pfCreateSound( pfDataLoad( file.c_str() ), false, false );
    return handle;
}

ruSoundHandle ruLoadSound3D( const string & file ) {
    ruSoundHandle handle;
    handle.pfHandle = pfCreateSound( pfDataLoad( file.c_str() ), true, false );
    return handle;
}

void ruFreeSound( ruSoundHandle sound ) {
    pfFreeSound( sound.pfHandle );
}

void ruSetAudioReverb( int reverb ) {
    pfSetListenerSetEAXPreset( (Preset)reverb );
}

ruSoundHandle ruLoadMusic( const string & file ) {
    ruSoundHandle handle;
    handle.pfHandle = pfCreateSound( pfDataLoad( file.c_str(), true ), false, false );
    return handle;
}

void ruSetSoundPosition( ruSoundHandle sound, ruVector3 pos ) {
    pfSetSoundPosition( sound.pfHandle, pos.x, pos.y, pos.z );
}

void ruSetSoundReferenceDistance( ruSoundHandle sound, float rd ) {
    pfSetReferenceDistance( sound.pfHandle, rd );
}

void ruSetSoundMaxDistance( ruSoundHandle sound, float maxDistance ) {
    pfSetMaxDistance( sound.pfHandle, maxDistance );
}

void ruSetRolloffFactor( ruSoundHandle sound, float rolloffFactor ) {
    pfSetRolloffFactor( sound.pfHandle, rolloffFactor );
};

void ruSetRoomRolloffFactor( ruSoundHandle sound, float rolloffDistance ) {
	pfSetRoomRolloffFactor( sound.pfHandle, rolloffDistance );
}

void ruPlaySound( ruSoundHandle sound, int oneshot ) {
    pfPlaySound( sound.pfHandle, oneshot );
}

void ruPauseSound( ruSoundHandle sound ) {
    pfPauseSound( sound.pfHandle );
}

void ruSetSoundVolume( ruSoundHandle sound, float vol ) {
    pfSetSoundVolume( sound.pfHandle, vol );
}

int ruIsSoundPlaying( ruSoundHandle sound ) {
    return pfIsSoundPlaying( sound.pfHandle );
}

RUAPI void ruSetMasterVolume( float volume ) {
    pfSetMasterVolume( volume );
}

RUAPI float ruGetMasterVolume() {
    return pfGetMasterVolume();
}

RUAPI void ruSetSoundsPitch( ruSoundHandle sound, float pitch ) {
    pfSetSoundPitch( sound.pfHandle, pitch );
}

RUAPI bool ruIsSoundPaused( ruSoundHandle sound ) {
    return pfIsSoundPlayingUntilPaused( sound.pfHandle );
}