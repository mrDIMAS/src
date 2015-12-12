/*******************************************************************************
*                               Ruthenium Engine                               *
*            Copyright (c) 2013-2016 Stepanov Dmitriy aka mrDIMAS              *
*                                                                              *
* This file is part of Ruthenium Engine.                                      *
*                                                                              *
* Ruthenium Engine is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* Ruthenium Engine is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with Ruthenium Engine.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                              *
*******************************************************************************/
#include "Precompiled.h"
#include "SceneNode.h"
#include "Engine.h"


SoundData GetSoundData( const string & fn, bool streamed ) {
	return pfDataLoad( fn.c_str(), streamed );
}

ruSound::ruSound() {
	pfHandle = -1;
}

ruSound::~ruSound() {

}

bool ruSound::IsValid() const {
	return pfIsSound( pfHandle );
}

void ruSound::Invalidate() {
	pfHandle = -1;
}

bool ruSound::operator == ( const ruSound & node ) {
	return pfHandle == node.pfHandle;
}

ruSound ruSound::Load2D( const string & file ) {
	ruSound handle;
	handle.pfHandle = pfCreateSound( GetSoundData( file, false ), false );
	return handle;
}

ruSound ruSound::Load3D( const string & file ) {
	ruSound handle;
	handle.pfHandle = pfCreateSound( GetSoundData( file, false ), true );
	return handle;
}

ruSound ruSound::LoadMusic( const string & file ) {
	ruSound handle;
	handle.pfHandle = pfCreateSound( GetSoundData( file, true ), false );
	return handle;
}

void ruSound::SetLoop( bool state ) {
	pfSetSoundLooping( pfHandle, state );
}

void ruSound::Attach( ruSceneNode node ) {
	if( SceneNode::CastHandle( node ) == nullptr ) {
		return;
	}
	SceneNode::CastHandle( node )->AttachSound( *this );
}

void ruSound::Play( int oneshot ) {
	pfPlaySound( pfHandle, oneshot );
}

float ruSound::GetLength() {
	return pfGetSoundLengthSeconds( pfHandle );
}

void ruSound::Pause() {
	pfPauseSound( pfHandle );
}

void ruSound::SetVolume( float vol ) {
	pfSetSoundVolume( pfHandle, vol );
}

void ruSound::SetPosition( ruVector3 pos ) {
	pfSetSoundPosition( pfHandle, pos.x, pos.y, pos.z );
}

void ruSound::SetReferenceDistance( float rd ) {
	pfSetReferenceDistance( pfHandle, rd );
}

void ruSound::SetRolloffFactor( float rolloffFactor ) {
	pfSetRolloffFactor( pfHandle, rolloffFactor );
}

void ruSound::SetRoomRolloffFactor( float rolloffDistance ) {
	pfSetRoomRolloffFactor( pfHandle, rolloffDistance );
}

void ruSound::SetMaxDistance( float maxDistance ) {
	pfSetMaxDistance( pfHandle, maxDistance );
}

int ruSound::IsPlaying() {
	return pfIsSoundPlaying( pfHandle );
}

void ruSound::Free() {
	pfFreeSound( pfHandle );
}

void ruSound::SetPitch( float pitch ) {
	pfSetSoundPitch( pfHandle, pitch );
}

bool ruSound::IsPaused() {
	return pfIsSoundPaused( pfHandle );
}

void ruSetAudioReverb( int reverb ) {
	pfSetListenerSetEAXPreset( (Preset)reverb );
}

void ruSetMasterVolume( float volume ) {
	pfSetMasterVolume( volume );
}

float ruGetMasterVolume() {
	return pfGetMasterVolume();
}

int ruSound::GetCount() {
	return pfSystemGetSoundCount();
}

ruSound ruSound::GetSound( int n ) {
	ruSound snd;
	snd.pfHandle = pfSystemGetSound( n );
	return snd;
}

float ruSound::GetPlaybackPosition( ) {
	return pfGetSoundPlaybackPosition( pfHandle );
}
void ruSound::SetPlaybackPosition( float timeSeconds ) {
	pfSetSoundPlaybackPosition( pfHandle, timeSeconds );
}


