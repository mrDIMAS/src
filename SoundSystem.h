#pragma once

class CSound : public ISound {
private:
	bool mIs3D;
	int pfHandle;
	friend class SoundSystem;
public:
	CSound(const string & fileName, bool is3D, bool isMusic);
	~CSound();
	virtual void SetLoop(bool state)  override final;
	virtual void Stop() override final;
	virtual bool Is3D() const override final;
	virtual Vector3 GetPosition() const override final;
	virtual void Attach(const shared_ptr<ISceneNode> & node)  override final;
	virtual void Play(int oneshot) override final;
	virtual float GetLength() override final;
	virtual void Pause() override final;
	virtual void SetVolume(float vol)  override final;
	virtual void SetPosition(Vector3 pos)  override final;
	virtual void SetReferenceDistance(float rd)  override final;
	virtual void SetRolloffFactor(float rolloffFactor) override final;
	virtual void SetRoomRolloffFactor(float rolloffDistance) override final;
	virtual void SetMaxDistance(float maxDistance)  override final;
	virtual int IsPlaying() override final;
	virtual void SetPitch(float pitch) override final;
	virtual bool IsPaused()  override final;
	virtual float GetPlaybackPosition()  override final;
	virtual void SetPlaybackPosition(float timeSeconds) override final;
};

class SoundSystem : public ISoundSystem {
private:
	vector<weak_ptr<ISound>> gSoundList;
public:
	SoundSystem();
	~SoundSystem();
	virtual void SetMasterVolume(float volume) override final;
	virtual float GetMasterVolume() override final;
	virtual void SetReverbPreset(ReverbPreset preset) override final;
	virtual int GetSoundCount() override final;
	virtual shared_ptr<ISound> GetSound(int i) override final;
	virtual shared_ptr<ISound> LoadSound2D(const string & file) override final;
	virtual shared_ptr<ISound> LoadSound3D(const string & file) override final;
	virtual shared_ptr<ISound> LoadMusic(const string & file) override final;
	virtual shared_ptr<ISound> LoadMusic3D(const string & file) override final;
};