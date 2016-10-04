#pragma once

#include "Game.h"

class SaveFile {
private:
	template<typename T>
	void Write(T v) {
		mStream.write((char*)&v, sizeof(v));
	}

	template<typename T>
	void Read(T & v) {
		mStream.read((char*)&v, sizeof(v));
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
		if (mSave) {
			Write(v);
		} else {
			Read(v);
		}
	}

	void operator & (float & v) {
		if (mSave) {
			Write(v);
		} else {
			Read(v);
		}
	}

	void operator & (bool & v) {
		if (mSave) {
			Write(v);
		} else {
			Read(v);
		}
	}

	void operator & (ruInput::Key & v) {
		int vi = (int)v;
		if (mSave) {
			Write(v);
		} else {
			Read(v);
		}
		v = (ruInput::Key)vi;
	}

	void operator & (string & str) {
		if (mSave) {
			for (auto symbol : str) {
				Write(symbol);
			}
			Write('\0');
		} else {
			while (!mStream.eof()) {
				char symbol;
				Read(symbol);
				if (symbol == '\0') {
					break;
				} else {
					str.push_back(symbol);
				}
			}
		}
	}

	void operator & (ruQuaternion & v) {
		*this & v.x;
		*this & v.y;
		*this & v.z;
		*this & v.w;
	}

	void operator & (ruVector3 & v) {
		*this & v.x;
		*this & v.y;
		*this & v.z;
	}

	void operator & (ruAnimation & a) {
		auto currentFrame = a.GetCurrentFrame();
		auto enabled = a.IsEnabled();

		*this & currentFrame;
		*this & enabled;
		
		a.SetCurrentFrame(currentFrame);
		a.SetEnabled(enabled);
	}

	template<typename K, typename V>
	void operator & (std::unordered_map<K, V> & m) {
		int s = m.size();
	    *this & s;
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