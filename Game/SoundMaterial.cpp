#include "Precompiled.h"
#include "SoundMaterial.h"

string GetFileLine(ifstream & s)
{
	const int bufferSize = 4096;
	char lineBuffer[bufferSize];
	s.getline(lineBuffer, bufferSize);
	return lineBuffer;
}

enum class ReaderMode {
	Unknown,
	Texture,
	Sound,
};

SoundMaterial::SoundMaterial(const string & filename, shared_ptr<ISceneNode> owner)
{
	auto soundSystem = owner->GetFactory()->GetEngineInterface()->GetSoundSystem();
	ifstream matFile(filename, ifstream::in);
	if(matFile.good()) {
		ReaderMode mode = ReaderMode::Unknown;
		float volume = 1.0f;
		while(!matFile.eof()) {
			string line = GetFileLine(matFile);
			if(line == ".snd") {
				mode = ReaderMode::Sound;
			} else if(line == ".tex") {
				mode = ReaderMode::Texture;
			} else if(line[0] == '#') {
				// # is a comment, ignore
			} else if(line.substr(0, 4) == ".vol") {
				volume = stof(line.substr(4, line.size() - 1));
			} else {
				if(line.size() > 2) {
					if(mode == ReaderMode::Sound) {
						shared_ptr<ISound> snd = soundSystem->LoadSound3D(line);
						snd->SetVolume(volume);
						snd->Attach(owner);
						mSoundList.push_back(snd);
					} else if(mode == ReaderMode::Texture) {
						mTextureList.push_back(line);
					}
				}
			}
		}
	}
}

shared_ptr<ISound> SoundMaterial::GetRandomSoundAssociatedWith(const string & texName)
{
	shared_ptr<ISound> snd;
	if(mSoundList.size()) {
		for(auto & str : mTextureList) {
			if(str == texName) {
				//try {
				snd = mSoundList.at(rand() % mSoundList.size());
				//} catch ( ... ) {
					// do nothing
				//}
				break;
			}
		}
	}
	return snd;
}

SoundMaterial::~SoundMaterial()
{

}

