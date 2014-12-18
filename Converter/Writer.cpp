#include "Writer.h"




void Writer::WriteQuaternion( Vector4 v )
{
	WriteFloat( v.x );
	WriteFloat( v.y );
	WriteFloat( v.z );
	WriteFloat( v.w );
}

void Writer::WriteVector2( Vector2 v )
{
	WriteFloat( v.x );
	WriteFloat( v.y );
}

void Writer::WriteVector( Vector3 v )
{
	WriteFloat( v.x );
	WriteFloat( v.y );
	WriteFloat( v.z );
}

void Writer::WriteString( string value )
{
	value.push_back( 0 ) ;
	output.write( (char*)&value[ 0 ], value.size() );
}

void Writer::WriteFloat( float value )
{
	output.write( (char*)&value, sizeof( value ));
}

void Writer::WriteShort( unsigned short value )
{
	output.write( (char*)&value, sizeof( value ));
}

void Writer::WriteInteger( int value )
{
	output.write( (char*)&value, sizeof( value ));
}

Writer::~Writer()
{
	output.close();
}

Writer::Writer( string fn )
{
	output.open( fn, ios::binary | ios::out );
}
