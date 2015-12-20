#pragma once

class SoundMaterial {
private:
	vector<shared_ptr<ruSound>> mSoundList;
	vector<string> mTextureList;
public:
	explicit SoundMaterial( const string & filename, shared_ptr<ruSceneNode> owner );
	shared_ptr<ruSound> GetRandomSoundAssociatedWith( const string & texName );
	~SoundMaterial();
};