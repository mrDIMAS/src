#include "Common.h"

Vector3::Vector3( ) {
    x = 0;
    y = 0;
    z = 0;
};

Vector3::Vector3( float x, float y, float z ) {
    this->x = x;
    this->y = y;
    this->z = z;
};

Vector3::Vector3( const Vector3 & v ) {
    x = v.x;
    y = v.y;
    z = v.z;
};

Vector3::Vector3( float * v )
{
    x = v[ 0 ];
    y = v[ 1 ];
    z = v[ 2 ];
}

Vector3 Vector3::operator + ( const Vector3 & v ) const {
    return Vector3( x + v.x, y + v.y, z + v.z );
}

Vector3 Vector3::operator - ( const Vector3 & v ) const {
    return Vector3( x - v.x, y - v.y, z - v.z );
}

Vector3 Vector3::operator * ( const Vector3 & v ) const {
    return Vector3( x * v.x, y * v.y, z * v.z );
}

Vector3 Vector3::operator * ( const float & f ) const {
    return Vector3( x * f, y * f, z * f );
}

Vector3 Vector3::operator / ( const Vector3 & v ) const {
    return Vector3( x / v.x, y / v.y, z / v.z );
}

Vector3 Vector3::operator / ( const float & f ) const {
    return Vector3( x / f, y / f, z / f );
}

void Vector3::operator *= ( const Vector3 & v ) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
}

void Vector3::operator /= ( const Vector3 & v ) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
}

void Vector3::operator += ( const Vector3 & v ) {
    x += v.x;
    y += v.y;
    z += v.z;
}

void Vector3::operator -= ( const Vector3 & v ) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

void Vector3::operator = ( const Vector3 & v ) {
    x = v.x;
    y = v.y;
    z = v.z;
}

bool Vector3::operator == ( const Vector3 & v ) {
    float dx = abs( x - v.x );
    float dy = abs( y - v.y );
    float dz = abs( z - v.z );

    return dx > 0.0001f && dy > 0.0001f && dz > 0.0001f;
}

float Vector3::Length( ) const {
    return sqrt( x * x + y * y + z * z );
}

float Vector3::Length2( ) const {
    return x * x + y * y + z * z;
}

Vector3 Vector3::Normalize( ) {
    float l = 1.0f / Length();

    x *= l;
    y *= l;
    z *= l;

    return *this;
}

Vector3 Vector3::Normalized() const {
    float l = 1.0f / Length();

    return Vector3( x * l, y * l, z * l );
}

Vector3 Vector3::Cross( const Vector3 & v ) const {
    return Vector3( y * v.z - z * v.x, z * v.x - x * v.z, x * v.y - y * v.x );
}

float Vector3::Dot( const Vector3 & v ) const {
    return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::Rotate( const Vector3 & axis, float angle ) {
    angle *= DEG2RAD;

    Vector3 o = axis * axis.Dot( *this );
    Vector3 x = *this - o;
    Vector3 y;

    y = axis.Cross( *this );

    return ( o + x * cosf( angle ) + y * sinf( angle ) );
}

Vector3 Vector3::Lerp( const Vector3 & v, float t ) const {
    return Vector3( x + ( v.x - x ) * t, y + ( v.y - y ) * t, z + ( v.z - z ) * t );
}