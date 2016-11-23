#include "Precompiled.h"
#include "SoundSystem.h"
#include "SceneNode.h"

SoundSystem::SoundSystem() {

}

SoundSystem::~SoundSystem() {

}

void SoundSystem::SetMasterVolume(float volume) {
	pfSetMasterVolume(volume);
}

float SoundSystem::GetMasterVolume() {
	return pfGetMasterVolume();
}

void SoundSystem::SetReverbPreset(ReverbPreset preset) {
	pfSetListenerSetEAXPreset((Preset)preset);
}

int SoundSystem::GetSoundCount() {
	return pfSystemGetSoundCount();
}

shared_ptr<ISound> SoundSystem::GetSound(int i) {
	return gSoundList[i].lock();
}

shared_ptr<ISound> SoundSystem::LoadSound2D(const string & file) {
	auto sound = make_shared<CSound>(file, false, false);
	gSoundList.push_back(sound);
	sound->mIs3D = false;
	return sound;
}

shared_ptr<ISound> SoundSystem::LoadSound3D(const string & file) {
	auto sound = make_shared<CSound>(file, true, false);
	gSoundList.push_back(sound);
	sound->mIs3D = true;
	return sound;
}

shared_ptr<ISound> SoundSystem::LoadMusic(const string & file) {
	auto sound = make_shared<CSound>(file, false, true);
	gSoundList.push_back(sound);
	sound->mIs3D = false;
	return sound;
}

CSound::CSound(const string & fileName, bool is3D, bool isMusic) : mIs3D(false) {
	pfHandle = pfCreateSound(pfDataLoad(fileName.c_str(), isMusic), is3D);
}

CSound::~CSound() {
	if(pfIsSound(pfHandle)) {
		pfFreeSound(pfHandle);
	}
}

void CSound::SetLoop(bool state) {
	pfSetSoundLooping(pfHandle, state);
}

void CSound::Stop() {
	pfStopSound(pfHandle);
}

bool CSound::Is3D() const {
	return mIs3D;
}

Vector3 CSound::GetPosition() const {
	Vector3 p;
	pfGetSoundPosition(pfHandle, (float*)&p);
	return p;
}

void CSound::Attach(const shared_ptr<ISceneNode>& node) {
	shared_ptr<SceneNode> & sceneNode = std::dynamic_pointer_cast<SceneNode>(node);
	if(sceneNode) {
		sceneNode->AttachSound(shared_from_this());
	}
}

void CSound::Play(int oneshot) {
	PlayCallback.PlayEvent();
	PlayCallback.Caller = shared_from_this();
	pfPlaySound(pfHandle, oneshot);
}

float CSound::GetLength() {
	return pfGetSoundLengthSeconds(pfHandle);
}

void CSound::Pause() {
	pfPauseSound(pfHandle);
}

void CSound::SetVolume(float vol) {
	pfSetSoundVolume(pfHandle, vol);
}

void CSound::SetPosition(Vector3 pos) {
	pfSetSoundPosition(pfHandle, pos.x, pos.y, pos.z);
}

void CSound::SetReferenceDistance(float rd) {
	pfSetReferenceDistance(pfHandle, rd);
}

void CSound::SetRolloffFactor(float rolloffFactor) {
	pfSetRolloffFactor(pfHandle, rolloffFactor);
}

void CSound::SetRoomRolloffFactor(float rolloffDistance) {
	pfSetRoomRolloffFactor(pfHandle, rolloffDistance);
}

void CSound::SetMaxDistance(float maxDistance) {
	pfSetMaxDistance(pfHandle, maxDistance);
}

int CSound::IsPlaying() {
	return pfIsSoundPlaying(pfHandle);
}

void CSound::SetPitch(float pitch) {
	pfSetSoundPitch(pfHandle, pitch);
}

bool CSound::IsPaused() {
	return pfIsSoundPaused(pfHandle);
}

float CSound::GetPlaybackPosition() {
	return pfGetSoundPlaybackPosition(pfHandle);
}

void CSound::SetPlaybackPosition(float timeSeconds) {
	pfSetSoundPlaybackPosition(pfHandle, timeSeconds);
}
