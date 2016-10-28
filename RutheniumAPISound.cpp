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
#include "Renderer.h"

vector<weak_ptr<ruSound>> gSoundList;
PlaybackCallback ruSound::PlayCallback;

SoundData GetSoundData( const string & fn, bool streamed ) {
	return pfDataLoad( fn.c_str(), streamed );
}

ruSound::ruSound() : mIs3D(false), pfHandle(-1) {

}

ruSound::~ruSound() {
	if( pfIsSound( pfHandle )) {
		pfFreeSound( pfHandle );
	}
}

shared_ptr<ruSound> ruSound::Load2D( const string & file ) {
	shared_ptr<ruSound> & handle = make_shared<ruSound>();
	handle->pfHandle = pfCreateSound( GetSoundData( file, false ), false );
	gSoundList.push_back( handle );
	handle->mIs3D = false;
	return std::move( handle );
}

shared_ptr<ruSound> ruSound::Load3D( const string & file ) {
	shared_ptr<ruSound> & handle = make_shared<ruSound>();
	handle->pfHandle = pfCreateSound( GetSoundData( file, false ), true );
	gSoundList.push_back( handle );
	handle->mIs3D = true;
	return std::move( handle );
}

shared_ptr<ruSound> ruSound::LoadMusic( const string & file ) {
	shared_ptr<ruSound> & handle = make_shared<ruSound>();
	handle->pfHandle = pfCreateSound( GetSoundData( file, true ), false );
	gSoundList.push_back( handle );
	return std::move( handle );
}

void ruSound::SetLoop( bool state ) {
	pfSetSoundLooping( pfHandle, state );
}

void ruSound::Stop() {
	pfStopSound( pfHandle );
}

bool ruSound::Is3D() const {
	return mIs3D;
}

ruVector3 ruSound::GetPosition() const {
	ruVector3 p;
	pfGetSoundPosition(pfHandle, (float*)&p);
	return p;
}

void ruSound::Attach( const shared_ptr<ruSceneNode> & node ) {
	shared_ptr<SceneNode> & sceneNode = std::dynamic_pointer_cast<SceneNode>( node );
	if( sceneNode ) {
		sceneNode->AttachSound( shared_from_this() );
	}
}

void ruSound::Play( int oneshot ) {
	PlayCallback.PlayEvent();
	PlayCallback.Caller = shared_from_this();
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

void ruSound::SetPitch( float pitch ) {
	pfSetSoundPitch( pfHandle, pitch );
}

bool ruSound::IsPaused() {
	return pfIsSoundPaused( pfHandle );
}

void ruSound::SetAudioReverb( int reverb ) {
	pfSetListenerSetEAXPreset( (Preset)reverb );
}

void ruSound::SetMasterVolume( float volume ) {
	pfSetMasterVolume( volume );
}

float ruSound::GetMasterVolume() {
	return pfGetMasterVolume();
}

int ruSound::GetCount() {
	return pfSystemGetSoundCount();
}

shared_ptr<ruSound> ruSound::GetSound( int n ) {
	return gSoundList[n].lock();
}

float ruSound::GetPlaybackPosition( ) {
	return pfGetSoundPlaybackPosition( pfHandle );
}
void ruSound::SetPlaybackPosition( float timeSeconds ) {
	pfSetSoundPlaybackPosition( pfHandle, timeSeconds );
}


