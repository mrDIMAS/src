#pragma once

class SoundMaterial {
private:
	vector<ruSound> mSoundList;
	vector<string> mTextureList;
public:
	explicit SoundMaterial( const string & filename, shared_ptr<ruSceneNode> owner );
	ruSound GetRandomSoundAssociatedWith( const string & texName );
	~SoundMaterial();
};