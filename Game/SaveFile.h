#pragma once

#include "Game.h"

class SaveFile {
private:
	template<typename T>
	void Handle(T & v) {
		if (mSave) {
			mStream << v << endl;
		} else {
			mStream >> v;
			mStream.ignore(1); // ingore \n
		}
	}

    fstream mStream;
	bool mSave;
public:
    SaveFile( const string & fileName, bool save );
    ~SaveFile();

	bool IsSaving() const {
		return mSave;
	}

	bool IsLoading() const {
		return !mSave;
	}

	void operator & (int & v) {
		Handle(v);
	}

	void operator & (float & v) {
		Handle(v);
	}

	void operator & (bool & v) {
		Handle(v);
	}

	void operator & (ruInput::Key & v) {
		int vi = (int)v;
		Handle(vi);
		v = (ruInput::Key)vi;
	}

	void operator & (string & v) {
		if (mSave) {
			mStream << v << endl;
		} else {
			getline(mStream, v);
		}
	}

	void operator & (ruQuaternion & v) {
		Handle(v.x);
		Handle(v.y);
		Handle(v.z);
		Handle(v.w);
	}

	void operator & (ruVector3 & v) {
		Handle(v.x);
		Handle(v.y);
		Handle(v.z);
	}

	void operator & (ruAnimation & a) {
		auto currentFrame = a.GetCurrentFrame();
		auto enabled = a.IsEnabled();

		Handle(currentFrame);
		Handle(enabled);
		
		a.SetCurrentFrame(currentFrame);
		a.SetEnabled(enabled);
	}

	template<typename K, typename V>
	void operator & (std::unordered_map<K, V> & m) {
		int s = m.size();
		Handle(s);
		if (IsLoading()) {
			for (int i = 0; i < s; ++i) {
				K key;
				V value;

				*this & key;
				*this & value;

				m[key] = value;
			}
		} else {
			for (auto p : m) {
				auto key = p.first;
				auto value = p.second;

				*this & key;
				*this & value;
			}
		}
	}
};