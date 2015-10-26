#pragma once

class SoundMaterial {
private:
	vector<ruSound> mSoundList;
	vector<string> mTextureList;
public:
	explicit SoundMaterial( const string & filename, ruSceneNode owner );
	ruSound GetRandomSoundAssociatedWith( const string & texName );
	~SoundMaterial();
};