#include "Vector.h"




Vector3 Vector3::Normalized()
{
	float len = sqrt( x*x + y*y + z*z );
	if( len > 0.000001 )
		return Vector3( x / len, y / len, z / len );
	else
		return Vector3( x, y, z );
}

float Vector3::Dot( const Vector3 & v )
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::Cross( const Vector3 & v )
{
	return Vector3( y * v.z - z * v.x, z * v.x - x * v.z, x * v.y - y * v.x );
}

