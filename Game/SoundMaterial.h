#pragma once

class SoundMaterial {
private:
	vector<shared_ptr<ISound>> mSoundList;
	vector<string> mTextureList;
public:
	explicit SoundMaterial(const string & filename, shared_ptr<ISceneNode> owner);
	shared_ptr<ISound> GetRandomSoundAssociatedWith(const string & texName);
	vector<shared_ptr<ISound>> & GetSoundList()
	{
		return mSoundList;
	}
	~SoundMaterial();
};