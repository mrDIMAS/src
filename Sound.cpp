#include "Common.h"

SoundHandle::SoundHandle()
{
  pfHandle = -1;
}

SoundHandle::~SoundHandle()
{

}

bool SoundHandle::IsValid()
{
  return pfHandle >= 0;
}

void SoundHandle::Invalidate()
{
  pfHandle = -1;
}

bool SoundHandle::operator == ( const SoundHandle & node )
{
  return pfHandle == node.pfHandle;
}

SoundHandle CreateSound2D( const char * file )
{
  SoundHandle handle;
  handle.pfHandle = pfCreateSound( pfDataLoad( file ), false, false );
  return handle;
}

SoundHandle CreateSound3D( const char * file )
{
  SoundHandle handle;
  handle.pfHandle = pfCreateSound( pfDataLoad( file ), true, false );
  return handle;
}

void FreeSoundSource( SoundHandle sound )
{
  pfFreeSound( sound.pfHandle );
}

void SetReverb( int reverb )
{
  pfSetListenerSetEAXPreset( (Preset)reverb );
}

SoundHandle CreateMusic( const char * file )
{
  SoundHandle handle;
  handle.pfHandle = pfCreateSound( pfDataLoad( file, true ), false, false );
  return handle;
}

void SetSoundPosition( SoundHandle sound, Vector3 pos )
{
  pfSetSoundPosition( sound.pfHandle, pos.x, pos.y, pos.z );
}

void SetSoundReferenceDistance( SoundHandle sound, float rd )
{
  pfSetReferenceDistance( sound.pfHandle, rd );
}

void SetRolloffFactor( float rolloffFactor )
{
  pfSetListenerRolloffFactor( rolloffFactor );
};

void PlaySoundSource( SoundHandle sound, int oneshot )
{
  pfPlaySound( sound.pfHandle, oneshot );
}

void PauseSoundSource( SoundHandle sound )
{
  pfPauseSound( sound.pfHandle );
}

void SetVolume( SoundHandle sound, float vol )
{
  pfSetSoundVolume( sound.pfHandle, vol );
}

int SoundPlaying( SoundHandle sound )
{
  return pfIsSoundPlaying( sound.pfHandle );
}

API void SetMasterVolume( float volume )
{
  pfSetMasterVolume( volume );
}

API float GetMasterVolume()
{
  return pfGetMasterVolume();
}
