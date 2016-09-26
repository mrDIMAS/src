#pragma once

#include "Common.h"
#include "Vector.h"

class Reader {
public:
	Reader();
	bool ReadFile(string fn);
	~Reader();
	int GetInteger();
	unsigned short GetShort();
	unsigned char GetByte();
	float GetFloat();
	string GetString();
	Vector3 GetVector();
	Vector2 GetBareVector2();
	Vector4 GetQuaternion();
	bool IsEnded();
private:
	long size;
	long cursor;
	typedef unsigned char Byte;
	Byte * data;
};