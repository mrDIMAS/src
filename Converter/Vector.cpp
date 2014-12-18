#include "Vector.h"




Vector3 Vector3::normalized()
{
	float len = sqrt( x*x + y*y + z*z );
	if( len > 0.000001 )
		return Vector3( x / len, y / len, z / len );
	else
		return Vector3( x, y, z );
}

float Vector3::dot( const Vector3 & v )
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::cross( const Vector3 & v )
{
	return Vector3( y * v.z - z * v.x, z * v.x - x * v.z, x * v.y - y * v.x );
}

Vector3::Vector3()
{

}

Vector3::Vector3( float _x, float _y, float _z )
{
	x = _x;
	y = _y;
	z = _z;
}
