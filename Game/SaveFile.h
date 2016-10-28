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

	bool IsSaving() const;
	bool IsLoading() const;
	void operator & (int & v);
	void operator & (float & v);
	void operator & (bool & v);
	void operator & (ruInput::Key & v);
	void operator & (string & str);
	void operator & (ruQuaternion & v);
	void operator & (ruVector3 & v);
	void operator & (ruAnimation & a);
	void operator & (class SmoothFloat & s);

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