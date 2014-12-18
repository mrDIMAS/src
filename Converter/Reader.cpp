#include "Reader.h"




bool Reader::IsEnded()
{
	return cursor > size;
}

Vector4 Reader::GetQuaternion()
{
	Vector4 v;

	v.x = GetFloat();
	v.y = GetFloat();
	v.z = GetFloat();
	v.w = GetFloat();

	return v;
}

Vector2 Reader::GetBareVector2()
{
	Vector2 v;

	v.x = GetFloat();
	v.y = GetFloat();

	return v;
}

Vector3 Reader::GetVector()
{
	Vector3 v;

	v.x = GetFloat();
	v.y = GetFloat();
	v.z = GetFloat();

	return v;
}

std::string Reader::GetString()
{
	string out;

	for ( ; data[ cursor ] != '\0'; ++cursor )
	{
		out += data[ cursor ];
	}

	if ( data[ cursor ] == '\0' )
		++cursor;

	return out;
}

float Reader::GetFloat()
{
	float value = * ( float * ) ( data + cursor );

	cursor += 4;

	return value;
}

unsigned char Reader::GetByte()
{
	unsigned char value = data[ cursor ];

	++cursor;

	return value;
}

unsigned short Reader::GetShort()
{
	unsigned short value = * ( unsigned short * ) ( data + cursor );

	cursor += 2;

	return value;
}

int Reader::GetInteger()
{
	int value = * ( int * ) ( data + cursor );

	cursor += 4;

	return value;
}

Reader::~Reader()
{
	delete data;

	data = 0;
}

bool Reader::ReadFile( string fn )
{
	FILE * f;
	fopen_s ( &f, fn.c_str(), "rb" );

	if ( !f )
	{
		return false;
	}

	fseek ( f, 0, SEEK_END );

	size = ftell ( f );

	data = new Byte[ size ];

	fseek ( f, 0, SEEK_SET );

	fread ( data, 1, size, f );

	fclose ( f );

	return true;
}

Reader::Reader()
{
	cursor = 0;
	data = 0;
}
