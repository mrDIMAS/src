#include "Precompiled.h"
#include "SaveFile.h"
#include "SmoothFloat.h"

SaveFile::~SaveFile() {
	mStream.flush();
	mStream.close();
}

SaveFile::SaveFile(const string & fileName, bool save) : mSave(save) {
	int flags = save ? (fstream::out | fstream::trunc | fstream::binary) : (fstream::in | fstream::binary);
	mStream.open(fileName, flags);
	if(!mStream.good()) {
		throw std::runtime_error(StringBuilder("Unable to open ") << fileName << " save file!");
	}
}

bool SaveFile::IsSaving() const {
	return mSave;
}

bool SaveFile::IsLoading() const {
	return !mSave;
}

void SaveFile::operator & (int & v) {
	if(mSave) {
		Write(v);
	} else {
		Read(v);
	}
}

void SaveFile::operator & (float & v) {
	if(mSave) {
		Write(v);
	} else {
		Read(v);
	}
}

void SaveFile::operator & (bool & v) {
	if(mSave) {
		Write(v);
	} else {
		Read(v);
	}
}

void SaveFile::operator & (IInput::Key & v) {
	int vi = (int)v;
	if(mSave) {
		Write(v);
	} else {
		Read(v);
	}
	v = (IInput::Key)vi;
}

void SaveFile::operator & (string & str) {
	if(mSave) {
		for(auto symbol : str) {
			Write(symbol);
		}
		Write('\0');
	} else {
		str.clear();
		while(!mStream.eof()) {
			char symbol;
			Read(symbol);
			if(symbol == '\0') {
				break;
			} else {
				str.push_back(symbol);
			}
		}
	}
}

void SaveFile::operator & (Quaternion & v) {
	*this & v.x;
	*this & v.y;
	*this & v.z;
	*this & v.w;
}

void SaveFile::operator & (Vector3 & v) {
	*this & v.x;
	*this & v.y;
	*this & v.z;
}

void SaveFile::operator & (Animation & a) {
	auto currentFrame = a.GetCurrentFrame();
	auto enabled = a.IsEnabled();
	auto interpolator = a.GetInterpolator();
	int direction = (int)a.GetDirection();

	*this & currentFrame;
	*this & enabled;
	*this & interpolator;
	*this & direction;

	a.SetCurrentFrame(currentFrame);
	a.SetEnabled(enabled);
	a.SetDirection((Animation::Direction)direction);
	a.SetInterpolator(interpolator);
}

void SaveFile::operator & (SmoothFloat & s) {
	s.Serialize(*this);
}